#include "DuplexReadSplitter.h"

#include "read_pipeline/read_utils.h"
#include "splitter/splitter_utils.h"
#include "utils/alignment_utils.h"
#include "utils/sequence_utils.h"
#include "utils/uuid_utils.h"

#include <ATen/ATen.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <optional>
#include <string>
#include <utility>

namespace {

using namespace dorado;
using namespace dorado::splitter;

using PosRange = splitter::PosRange;
using PosRanges = splitter::PosRanges;

//[start, end)
std::optional<PosRange> find_best_adapter_match(const std::string& adapter,
                                                const std::string& seq,
                                                int dist_thr,
                                                PosRange subrange) {
    assert(subrange.first <= subrange.second && subrange.second <= seq.size());
    auto shift = subrange.first;
    auto span = subrange.second - subrange.first;

    if (span == 0)
        return std::nullopt;

    auto edlib_cfg = edlibNewAlignConfig(dist_thr, EDLIB_MODE_HW, EDLIB_TASK_LOC, NULL, 0);

    auto edlib_result = edlibAlign(adapter.c_str(), int(adapter.size()), seq.c_str() + shift,
                                   int(span), edlib_cfg);
    assert(edlib_result.status == EDLIB_STATUS_OK);
    std::optional<PosRange> res = std::nullopt;
    if (edlib_result.status == EDLIB_STATUS_OK && edlib_result.editDistance != -1) {
        assert(edlib_result.editDistance <= dist_thr);
        res = {edlib_result.startLocations[0] + shift, edlib_result.endLocations[0] + shift + 1};
    }
    edlibFreeAlignResult(edlib_result);
    return res;
}

//currently just finds a single best match
//TODO efficiently find more matches
std::vector<PosRange> find_adapter_matches(const std::string& adapter,
                                           const std::string& seq,
                                           int dist_thr,
                                           uint64_t ignore_prefix) {
    std::vector<PosRange> answer;
    if (ignore_prefix < seq.size()) {
        if (auto best_match =
                    find_best_adapter_match(adapter, seq, dist_thr, {ignore_prefix, seq.size()})) {
            answer.push_back(*best_match);
        }
    }
    return answer;
}

//semi-global alignment of "template region" to "complement region"
//returns range in the compl_r
std::optional<PosRange> check_rc_match(const std::string& seq,
                                       PosRange templ_r,
                                       PosRange compl_r,
                                       int dist_thr) {
    assert(templ_r.second > templ_r.first);
    assert(compl_r.second > compl_r.first);
    assert(dist_thr >= 0);

    auto rc_compl = dorado::utils::reverse_complement(
            seq.substr(compl_r.first, compl_r.second - compl_r.first));

    auto edlib_cfg = edlibNewAlignConfig(dist_thr, EDLIB_MODE_HW, EDLIB_TASK_LOC, NULL, 0);

    auto edlib_result = edlibAlign(seq.c_str() + templ_r.first, int(templ_r.second - templ_r.first),
                                   rc_compl.c_str(), int(rc_compl.size()), edlib_cfg);
    assert(edlib_result.status == EDLIB_STATUS_OK);

    bool match = (edlib_result.status == EDLIB_STATUS_OK) && (edlib_result.editDistance != -1);
    std::optional<PosRange> res = std::nullopt;
    if (match) {
        assert(edlib_result.editDistance <= dist_thr);
        assert(edlib_result.numLocations > 0 &&
               edlib_result.endLocations[0] < int(compl_r.second) &&
               edlib_result.startLocations[0] < int(compl_r.second));
        res = PosRange(compl_r.second - edlib_result.endLocations[0],
                       compl_r.second - edlib_result.startLocations[0]);
    }

    edlibFreeAlignResult(edlib_result);
    return res;
}

// NB: Computes literal mean of the qscore values, not generally applicable
float qscore_mean(const std::string& qstring, splitter::PosRange r) {
    uint64_t len = qstring.size();
    uint64_t start = r.first;
    uint64_t end = std::min(r.second, len);
    assert(start < end);
    uint64_t sum = 0;
    for (size_t i = start; i < end; ++i) {
        assert(qstring[i] > 33);
        sum += qstring[i] - 33;
    }
    return float(1. * sum / (end - start));
}

}  // namespace

namespace dorado::splitter {

struct DuplexReadSplitter::ExtRead {
    SimplexReadPtr read;
    at::Tensor data_as_float32;
    std::vector<uint64_t> move_sums;
    splitter::PosRanges possible_pore_regions;
};

DuplexReadSplitter::ExtRead DuplexReadSplitter::create_ext_read(SimplexReadPtr r) const {
    ExtRead ext_read;
    ext_read.read = std::move(r);
    ext_read.move_sums = utils::move_cum_sums(ext_read.read->read_common.moves);
    assert(!ext_read.move_sums.empty());
    assert(ext_read.move_sums.back() == ext_read.read->read_common.seq.length());
    ext_read.data_as_float32 = ext_read.read->read_common.raw_data.to(at::kFloat);
    ext_read.possible_pore_regions = possible_pore_regions(ext_read);
    return ext_read;
}

PosRanges DuplexReadSplitter::possible_pore_regions(const DuplexReadSplitter::ExtRead& read) const {
    spdlog::trace("Analyzing signal in read {}", read.read->read_common.read_id);

    auto pore_sample_ranges =
            detect_pore_signal<float>(read.data_as_float32, m_settings.pore_thr,
                                      m_settings.pore_cl_dist, m_settings.expect_pore_prefix);

    std::vector<std::pair<float, PosRange>> candidate_regions;
    for (auto pore_sample_range : pore_sample_ranges) {
        auto move_start = pore_sample_range.start_sample / read.read->read_common.model_stride;
        auto move_end = pore_sample_range.end_sample / read.read->read_common.model_stride;
        auto move_argmax = pore_sample_range.argmax_sample / read.read->read_common.model_stride;
        assert(move_end >= move_argmax && move_argmax >= move_start);
        if (move_end >= read.move_sums.size() || read.move_sums[move_start] == 0) {
            //either at very end of the signal or basecalls have not started yet
            continue;
        }
        auto start_pos = read.move_sums[move_start] - 1;
        //TODO check (- 1)
        auto argmax_pos = read.move_sums[move_argmax] - 1;
        auto end_pos = read.move_sums[move_end];
        //check that detected cluster corresponds to not too many bases
        if (end_pos > start_pos + m_settings.max_pore_region) {
            continue;
        }

        assert(end_pos > argmax_pos && argmax_pos >= start_pos);
        if (m_settings.use_argmax) {
            //switch to position of max sample
            start_pos = argmax_pos;
            end_pos = argmax_pos + 1;
        }

        //check that mean qscore near pore is low
        if (m_settings.qscore_check_span > 0 &&
            qscore_mean(read.read->read_common.qstring,
                        {start_pos, start_pos + m_settings.qscore_check_span}) >
                    m_settings.mean_qscore_thr - std::numeric_limits<float>::epsilon()) {
            continue;
        }
        candidate_regions.push_back({pore_sample_range.max_val, {start_pos, end_pos}});
    }

    //sorting by max signal value within the cluster
    std::sort(candidate_regions.begin(), candidate_regions.end());
    //take top candidates
    PosRanges pore_regions;
    for (size_t i = std::max(int64_t(candidate_regions.size()) - m_settings.top_candidates,
                             int64_t(0));
         i < candidate_regions.size(); ++i) {
        pore_regions.push_back(candidate_regions[i].second);
    }
    //sorting by first coordinate again
    std::sort(pore_regions.begin(), pore_regions.end());

    spdlog::trace("Detected {} potential pore regions in read {}", pore_regions.size(),
                  read.read->read_common.read_id);
    return pore_regions;
}

bool DuplexReadSplitter::check_nearby_adapter(const SimplexRead& read,
                                              PosRange r,
                                              int adapter_edist) const {
    return find_best_adapter_match(m_settings.adapter, read.read_common.seq, adapter_edist,
                                   //including spacer region in search
                                   {r.first, std::min(r.second + m_settings.pore_adapter_span,
                                                      (uint64_t)read.read_common.seq.size())})
            .has_value();
}

//'spacer' is region potentially containing templ/compl strand boundary
//returns optional pair of matching ranges (first strictly to the left of spacer region)
std::optional<std::pair<PosRange, PosRange>> DuplexReadSplitter::check_flank_match(
        const SimplexRead& read,
        PosRange spacer,
        float err_thr) const {
    const uint64_t rlen = read.read_common.seq.length();
    assert(spacer.first <= spacer.second && spacer.second <= rlen);
    if (spacer.first <= m_settings.strand_end_trim || spacer.second == rlen) {
        return std::nullopt;
    }

    const uint64_t left_start = (spacer.first > m_settings.strand_end_flank)
                                        ? spacer.first - m_settings.strand_end_flank
                                        : 0;
    const uint64_t left_end = spacer.first - m_settings.strand_end_trim;
    assert(left_start < left_end);
    const uint64_t left_span = left_end - left_start;

    //including spacer region in search
    const uint64_t right_start = spacer.first;
    //(r.second - r.first) adjusts for potentially incorrectly detected split region
    //, shifting into correct sequence
    const uint64_t right_end = std::min(
            spacer.second + m_settings.strand_start_flank + (spacer.second - spacer.first), rlen);
    assert(right_start < right_end);
    const uint64_t right_span = right_end - right_start;

    const int dist_thr = int(std::round(err_thr * left_span));
    if (left_span >= m_settings.min_flank && right_span >= left_span) {
        if (auto match = check_rc_match(read.read_common.seq, {left_start, left_end},
                                        //including spacer region in search
                                        {right_start, right_end}, dist_thr)) {
            return std::pair{PosRange{left_start, left_end}, *match};
        }
    }
    return std::nullopt;
}

std::optional<PosRange> DuplexReadSplitter::identify_middle_adapter_split(
        const SimplexRead& read) const {
    assert(m_settings.strand_end_flank > m_settings.strand_end_trim + m_settings.min_flank);
    const uint64_t r_l = read.read_common.seq.size();
    const uint64_t search_span =
            std::max(m_settings.middle_adapter_search_span,
                     uint64_t(std::round(m_settings.middle_adapter_search_frac * r_l)));
    if (r_l < search_span) {
        return std::nullopt;
    }

    spdlog::trace("Searching for adapter match");
    if (auto adapter_match = find_best_adapter_match(
                m_settings.adapter, read.read_common.seq, m_settings.relaxed_adapter_edist,
                {r_l / 2 - search_span / 2, r_l / 2 + search_span / 2})) {
        const uint64_t adapter_start = adapter_match->first;
        const uint64_t adapter_end = adapter_match->second;
        spdlog::trace("Checking middle match & start/end match");
        //Checking match around adapter
        if (check_flank_match(read, {adapter_start, adapter_start}, m_settings.flank_err)) {
            //Checking start/end match
            //some initializations might 'overflow' and not make sense, but not if check_rc_match below actually ends up checked!
            const uint64_t query_start = r_l - m_settings.strand_end_flank;
            const uint64_t query_end = r_l - m_settings.strand_end_trim;
            const uint64_t query_span = query_end - query_start;
            const int dist_thr = int(std::round(m_settings.flank_err * query_span));

            const uint64_t template_start = 0;
            const uint64_t template_end = std::min(m_settings.strand_start_flank, adapter_start);
            const uint64_t template_span = template_end - template_start;

            if (adapter_end + m_settings.strand_end_flank > r_l || template_span < query_span ||
                check_rc_match(
                        read.read_common.seq,
                        {r_l - m_settings.strand_end_flank, r_l - m_settings.strand_end_trim},
                        {0, std::min(m_settings.strand_start_flank, r_l)}, dist_thr)) {
                return PosRange{adapter_start - 1, adapter_start};
            }
        }
    }
    return std::nullopt;
}

std::optional<PosRange> DuplexReadSplitter::identify_extra_middle_split(
        const SimplexRead& read) const {
    const uint64_t r_l = read.read_common.seq.size();
    //TODO parameterize
    const float ext_start_frac = 0.1f;
    //extend to tolerate some extra length difference
    const uint64_t ext_start_flank =
            std::max(uint64_t(ext_start_frac * r_l), m_settings.strand_start_flank);
    //further consider only reasonably long reads
    if (ext_start_flank + m_settings.strand_end_flank > r_l) {
        return std::nullopt;
    }

    int flank_edist = int(std::round(m_settings.flank_err *
                                     (m_settings.strand_end_flank - m_settings.strand_end_trim)));

    spdlog::trace("Checking start/end match");
    if (auto templ_start_match = check_rc_match(
                read.read_common.seq,
                {r_l - m_settings.strand_end_flank, r_l - m_settings.strand_end_trim},
                {0, std::min(r_l, ext_start_flank)}, flank_edist)) {
        //check if matched region and query overlap
        if (templ_start_match->second + m_settings.strand_end_flank > r_l) {
            return std::nullopt;
        }
        uint64_t est_middle = (templ_start_match->second + (r_l - m_settings.strand_end_flank)) / 2;
        spdlog::trace("Middle estimate {}", est_middle);
        //TODO parameterize
        const int min_split_margin = 100;
        const float split_margin_frac = 0.05f;
        const auto split_margin = std::max(min_split_margin, int(split_margin_frac * r_l));

        spdlog::trace("Checking approx middle match");
        if (auto middle_match_ranges =
                    check_flank_match(read, {est_middle - split_margin, est_middle + split_margin},
                                      m_settings.flank_err)) {
            est_middle =
                    (middle_match_ranges->first.second + middle_match_ranges->second.first) / 2;
            spdlog::trace("Middle re-estimate {}", est_middle);
            return PosRange{est_middle - 1, est_middle};
        }
    }
    return std::nullopt;
}

std::vector<SimplexReadPtr> DuplexReadSplitter::subreads(SimplexReadPtr read,
                                                         const PosRanges& spacers) const {
    std::vector<SimplexReadPtr> subreads;
    subreads.reserve(spacers.size() + 1);

    if (spacers.empty()) {
        subreads.push_back(std::move(read));
        return subreads;
    }

    const auto stride = read->read_common.model_stride;
    const auto seq_to_sig_map = utils::moves_to_map(read->read_common.moves, stride,
                                                    read->read_common.get_raw_data_samples(),
                                                    read->read_common.seq.size() + 1);

    //TODO maybe simplify by adding begin/end stubs?
    uint64_t start_pos = 0;
    uint64_t signal_start = seq_to_sig_map[0];
    for (auto r : spacers) {
        if (start_pos < r.first && signal_start / stride < seq_to_sig_map[r.first] / stride) {
            subreads.push_back(subread(*read, PosRange{start_pos, r.first},
                                       PosRange{signal_start, seq_to_sig_map[r.first]}));
        }
        start_pos = r.second;
        signal_start = seq_to_sig_map[r.second];
    }
    assert(read->read_common.get_raw_data_samples() ==
           seq_to_sig_map[read->read_common.seq.size()]);
    if (start_pos < read->read_common.seq.size() &&
        signal_start / stride < read->read_common.get_raw_data_samples() / stride) {
        subreads.push_back(
                subread(*read, PosRange{start_pos, read->read_common.seq.size()},
                        PosRange{signal_start, read->read_common.get_raw_data_samples()}));
    }

    return subreads;
}

std::vector<std::pair<std::string, DuplexReadSplitter::SplitFinderF>>
DuplexReadSplitter::build_split_finders() const {
    std::vector<std::pair<std::string, SplitFinderF>> split_finders;
    split_finders.push_back({"PORE_ADAPTER", [&](const ExtRead& read) {
                                 return filter_ranges(read.possible_pore_regions, [&](PosRange r) {
                                     return check_nearby_adapter(*read.read, r,
                                                                 m_settings.adapter_edist);
                                 });
                             }});

    if (!m_settings.simplex_mode) {
        split_finders.push_back(
                {"PORE_FLANK", [&](const ExtRead& read) {
                     return merge_ranges(
                             filter_ranges(read.possible_pore_regions,
                                           [&](PosRange r) {
                                               return check_flank_match(*read.read, r,
                                                                        m_settings.flank_err);
                                           }),
                             m_settings.strand_end_flank + m_settings.strand_start_flank);
                 }});

        split_finders.push_back(
                {"PORE_ALL", [&](const ExtRead& read) {
                     return merge_ranges(
                             filter_ranges(read.possible_pore_regions,
                                           [&](PosRange r) {
                                               return check_nearby_adapter(
                                                              *read.read, r,
                                                              m_settings.relaxed_adapter_edist) &&
                                                      check_flank_match(
                                                              *read.read, r,
                                                              m_settings.relaxed_flank_err);
                                           }),
                             m_settings.strand_end_flank + m_settings.strand_start_flank);
                 }});

        split_finders.push_back(
                {"ADAPTER_FLANK", [&](const ExtRead& read) {
                     return filter_ranges(
                             find_adapter_matches(m_settings.adapter, read.read->read_common.seq,
                                                  m_settings.adapter_edist,
                                                  m_settings.expect_adapter_prefix),
                             [&](PosRange r) {
                                 return check_flank_match(*read.read, {r.first, r.first},
                                                          m_settings.flank_err);
                             });
                 }});

        split_finders.push_back({"ADAPTER_MIDDLE", [&](const ExtRead& read) {
                                     if (auto split = identify_middle_adapter_split(*read.read)) {
                                         return PosRanges{*split};
                                     } else {
                                         return PosRanges();
                                     }
                                 }});

        split_finders.push_back({"SPLIT_MIDDLE", [&](const ExtRead& read) {
                                     if (auto split = identify_extra_middle_split(*read.read)) {
                                         return PosRanges{*split};
                                     } else {
                                         return PosRanges();
                                     }
                                 }});
    }

    return split_finders;
}

std::vector<SimplexReadPtr> DuplexReadSplitter::split(SimplexReadPtr init_read) const {
    using namespace std::chrono;

    auto start_ts = high_resolution_clock::now();
    auto read_id = init_read->read_common.read_id;
    spdlog::trace("Processing read {}; length {}", read_id, init_read->read_common.seq.size());

    //assert(!init_read->seq.empty() && !init_read->moves.empty());
    if (init_read->read_common.seq.empty() || init_read->read_common.moves.empty()) {
        spdlog::trace("Empty read {}; length {}; moves {}", read_id,
                      init_read->read_common.seq.size(), init_read->read_common.moves.size());
        std::vector<SimplexReadPtr> split_result;
        split_result.push_back(std::move(init_read));
        return split_result;
    }

    std::vector<ExtRead> to_split;
    to_split.push_back(create_ext_read(std::move(init_read)));
    for (const auto& [description, split_f] : m_split_finders) {
        spdlog::trace("Running {}", description);
        std::vector<ExtRead> split_round_result;
        for (auto& r : to_split) {
            auto spacers = split_f(r);
            spdlog::trace("DSN: {} strategy {} splits in read {}", description, spacers.size(),
                          read_id);

            if (spacers.empty()) {
                split_round_result.push_back(std::move(r));
            } else {
                for (auto& sr : subreads(std::move(r.read), spacers)) {
                    split_round_result.push_back(create_ext_read(std::move(sr)));
                }
            }
        }
        to_split = std::move(split_round_result);
    }

    std::vector<SimplexReadPtr> split_result;
    size_t subread_id = 0;
    for (auto& ext_read : to_split) {
        if (!ext_read.read->read_common.parent_read_id.empty()) {
            ext_read.read->read_common.subread_id = subread_id++;
            ext_read.read->read_common.split_count = to_split.size();
            const auto subread_uuid =
                    utils::derive_uuid(ext_read.read->read_common.parent_read_id,
                                       std::to_string(ext_read.read->read_common.subread_id));
            ext_read.read->read_common.read_id = subread_uuid;
        }

        split_result.push_back(std::move(ext_read.read));
    }

    // Adjust prev and next read ids.
    if (split_result.size() > 1) {
        for (size_t i = 0; i < split_result.size(); i++) {
            if (i == split_result.size() - 1) {
                // For the last split read, the next read remains the same as the
                // original read's next read.
                split_result[i]->prev_read = split_result[i - 1]->read_common.read_id;
            } else if (i == 0) {
                // For the first split read, the previous read remains the same as the
                // original read's previous read.
                split_result[i]->next_read = split_result[i + 1]->read_common.read_id;
            } else {
                split_result[i]->prev_read = split_result[i - 1]->read_common.read_id;
                split_result[i]->next_read = split_result[i + 1]->read_common.read_id;
            }
        }
    }

    spdlog::trace("Read {} split into {} subreads", read_id, split_result.size());

    auto stop_ts = high_resolution_clock::now();
    spdlog::trace("READ duration: {} microseconds (ID: {})",
                  duration_cast<microseconds>(stop_ts - start_ts).count(), read_id);

    return split_result;
}

DuplexReadSplitter::DuplexReadSplitter(DuplexSplitSettings settings)
        : m_settings(std::move(settings)) {
    m_split_finders = build_split_finders();
}

DuplexReadSplitter::~DuplexReadSplitter() {}

}  // namespace dorado::splitter

#include "AdapterDetector.h"

#include "utils/alignment_utils.h"
#include "utils/sequence_utils.h"
#include "utils/types.h"

#include <edlib.h>
#include <htslib/sam.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

namespace {

const int ADAPTER_TRIM_LENGTH = 75;
const int PRIMER_TRIM_LENGTH = 150;

// Create edlib configuration for detecting adapters and primers.
EdlibAlignConfig init_edlib_config_for_adapters() {
    EdlibAlignConfig placement_config = edlibDefaultAlignConfig();
    placement_config.mode = EDLIB_MODE_HW;
    placement_config.task = EDLIB_TASK_PATH;
    // Currently none of our adapters or primers have Ns, but we should support them.
    static const EdlibEqualityPair additionalEqualities[4] = {
            {'N', 'A'}, {'N', 'T'}, {'N', 'C'}, {'N', 'G'}};
    placement_config.additionalEqualities = additionalEqualities;
    placement_config.additionalEqualitiesLength = 4;
    return placement_config;
}

// For adapters, we there are specific sequences we look for at the front of the read. We don't look for exactly
// the reverse complement at the rear of the read, though, because it will generally be truncated. So we list here
// the specific sequences to look for at the front and rear of the reads.
struct Adapter {
    std::string name;
    std::string front_sequence;
    std::string rear_sequence;
};

const std::vector<Adapter> adapters = {
        {"LSK109", "AATGTACTTCGTTCAGTTACGTATTGCT", "AGCAATACGTAACTGAACGAAGT"},
        {"LSK110", "CCTGTACTTCGTTCAGTTACGTATTGC", "AGCAATACGTAACTGAAC"}};

// For primers, we look for each primer sequence, and its reverse complement, at both the front and rear of the read.
struct Primer {
    std::string name;
    std::string sequence;
};

const std::vector<Primer> primers = {
        {"PCR_PSK_rev1", "ACTTGCCTGTCGCTCTATCTTCGGCGTCTGCTTGGGTGTTTAACC"},
        {"PCR_PSK_rev2", "TTTCTGTTGGTGCTGATATTGCGGCGTCTGCTTGGGTGTTTAACCT"},
        {"cDNA_VNP", "ACTTGCCTGTCGCTCTATCTTC"},
        {"cDNA_SSP", "TTTCTGTTGGTGCTGATATTGCTGGG"},
        {"PCS110_forward",
         "TCGCCTACCGTGACAAGAAAGTTGTCGGTGTCTTTGTGACTTGCCTGTCGCTCTATCTTCAGAGGAGAGTCCGCCGCCCGCAAGTTT"},
        {"PCS110_reverse", "ATCGCCTACCGTGACAAGAAAGTTGTCGGTGTCTTTGTGTTTCTGTTGGTGCTGATATTGCTTT"},
        // Not included because it is too similar to RBK barcode flank
        // {"RAD", "GCTTGGGTGTTTAACCGTTTTCGCATTTATCGTGAAACGCTTTCGCGTTTTTCGTGCGCCGCTTCA"}
};

}  // namespace

namespace dorado {
namespace demux {

AdapterDetector::AdapterDetector() {
    m_adapter_sequences.resize(adapters.size());
    for (size_t i = 0; i < adapters.size(); ++i) {
        m_adapter_sequences[i].name = adapters[i].name;
        m_adapter_sequences[i].sequence = adapters[i].front_sequence;
        m_adapter_sequences[i].sequence_rev = adapters[i].rear_sequence;
    }
    m_primer_sequences.resize(primers.size());
    for (size_t i = 0; i < primers.size(); ++i) {
        m_primer_sequences[i].name = primers[i].name;
        m_primer_sequences[i].sequence = primers[i].sequence;
        m_primer_sequences[i].sequence_rev = utils::reverse_complement(primers[i].sequence);
    }
}

AdapterDetector::~AdapterDetector() = default;

AdapterScoreResult AdapterDetector::find_adapters(const std::string& seq) {
    return detect(seq, m_adapter_sequences, ADAPTER);
}

AdapterScoreResult AdapterDetector::find_primers(const std::string& seq) {
    return detect(seq, m_primer_sequences, PRIMER);
}

const std::vector<AdapterDetector::Query>& AdapterDetector::get_adapter_sequences() const {
    return m_adapter_sequences;
}

const std::vector<AdapterDetector::Query>& AdapterDetector::get_primer_sequences() const {
    return m_primer_sequences;
}

static SingleEndResult copy_results(const EdlibAlignResult& source,
                                    const std::string& name,
                                    size_t length) {
    SingleEndResult dest;
    dest.name = name;
    dest.score = 1.0f - float(source.editDistance) / length;
    dest.position = {source.startLocations[0], source.endLocations[0]};
    return dest;
}

AdapterScoreResult AdapterDetector::detect(const std::string& seq,
                                           const std::vector<Query>& queries,
                                           AdapterDetector::QueryType query_type) const {
    const std::string_view seq_view(seq);
    const auto TRIM_LENGTH = (query_type == ADAPTER ? ADAPTER_TRIM_LENGTH : PRIMER_TRIM_LENGTH);
    const std::string_view read_front = seq_view.substr(0, TRIM_LENGTH);
    int rear_start = std::max(0, int(seq.length()) - TRIM_LENGTH);
    const std::string_view read_rear = seq_view.substr(rear_start, TRIM_LENGTH);

    // Try to find the location of the queries in the front and rear windows.
    EdlibAlignConfig placement_config = init_edlib_config_for_adapters();

    std::vector<SingleEndResult> front_results, rear_results;
    for (size_t i = 0; i < queries.size(); i++) {
        const auto& name = queries[i].name;
        const auto& query_seq = queries[i].sequence;
        const auto& query_seq_rev = queries[i].sequence_rev;
        spdlog::trace("Checking adapter/primer {}", name);

        auto front_result = edlibAlign(query_seq.data(), int(query_seq.length()), read_front.data(),
                                       int(read_front.length()), placement_config);
        front_results.emplace_back(copy_results(front_result, name + "_FWD", query_seq.length()));
        edlibFreeAlignResult(front_result);
        if (query_type == PRIMER) {
            // For primers we look for both the forward and reverse sequence at both ends.
            auto front_result_rev =
                    edlibAlign(query_seq_rev.data(), int(query_seq_rev.length()), read_front.data(),
                               int(read_front.length()), placement_config);
            front_results.emplace_back(
                    copy_results(front_result_rev, name + "_REV", query_seq_rev.length()));
            edlibFreeAlignResult(front_result_rev);
        }

        auto rear_result = edlibAlign(query_seq_rev.data(), int(query_seq_rev.length()),
                                      read_rear.data(), int(read_rear.length()), placement_config);
        rear_results.emplace_back(copy_results(rear_result, name + "_REV", query_seq_rev.length()));
        rear_results.back().position.first += rear_start;
        rear_results.back().position.second += rear_start;
        edlibFreeAlignResult(rear_result);
        if (query_type == PRIMER) {
            auto rear_result_fwd =
                    edlibAlign(query_seq.data(), int(query_seq.length()), read_rear.data(),
                               int(read_rear.length()), placement_config);
            rear_results.emplace_back(
                    copy_results(rear_result_fwd, name + "_FWD", query_seq.length()));
            rear_results.back().position.first += rear_start;
            rear_results.back().position.second += rear_start;
            edlibFreeAlignResult(rear_result_fwd);
        }
    }
    int best_front = -1, best_rear = -1;
    float best_front_score = -1.0f, best_rear_score = -1.0f;
    const float EPSILON = 0.1f;
    AdapterScoreResult result;
    for (size_t i = 0; i < front_results.size(); ++i) {
        int old_span = (best_front == -1) ? 0
                                          : front_results[best_front].position.second -
                                                    front_results[best_front].position.first;
        int new_span = front_results[i].position.second - front_results[i].position.first;
        if (front_results[i].score > best_front_score + EPSILON) {
            // The current match is clearly better than the previously seen best match.
            best_front_score = front_results[i].score;
            best_front = int(i);
        }
        if (std::abs(front_results[i].score - best_front_score) <= EPSILON) {
            // The current match and previously seen best match have nearly equal scores. Pick the longer one.
            if (new_span > old_span) {
                best_front_score = front_results[i].score;
                best_front = int(i);
            }
        }
    }
    for (size_t i = 0; i < rear_results.size(); ++i) {
        int old_span = (best_rear == -1) ? 0
                                         : rear_results[best_rear].position.second -
                                                   rear_results[best_rear].position.first;
        int new_span = rear_results[i].position.second - rear_results[i].position.first;
        if (rear_results[i].score > best_rear_score + EPSILON) {
            // The current match is clearly better than the previously seen best match.
            best_rear_score = rear_results[i].score;
            best_rear = int(i);
        }
        if (std::abs(rear_results[i].score - best_rear_score) <= EPSILON) {
            // The current match and previously seen best match have nearly equal scores. Pick the longer one.
            if (new_span > old_span) {
                best_rear_score = rear_results[i].score;
                best_rear = int(i);
            }
        }
    }
    result.front = front_results[best_front];
    result.rear = rear_results[best_rear];
    return result;
}

}  // namespace demux
}  // namespace dorado

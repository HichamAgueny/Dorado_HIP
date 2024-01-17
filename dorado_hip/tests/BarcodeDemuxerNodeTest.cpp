#include "read_pipeline/BarcodeDemuxerNode.h"

#include "MessageSinkUtils.h"
#include "TestUtils.h"
#include "read_pipeline/HtsReader.h"
#include "utils/SampleSheet.h"
#include "utils/bam_utils.h"
#include "utils/sequence_utils.h"
#include "utils/types.h"

#include <catch2/catch.hpp>
#include <htslib/sam.h>

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#define TEST_GROUP "[barcode_demux]"

namespace fs = std::filesystem;

using namespace dorado;

namespace {
std::vector<BamPtr> create_bam_reader(const std::string& bc) {
    ReadCommon read_common;
    read_common.seq = "AAAA";
    read_common.qstring = "!!!!";
    read_common.read_id = bc;
    auto records = read_common.extract_sam_lines(false, 0, false);
    for (auto& rec : records) {
        bam_aux_append(rec.get(), "BC", 'Z', int(bc.length() + 1), (uint8_t*)bc.c_str());
    }
    return records;
}
}  // namespace

TEST_CASE("BarcodeDemuxerNode: check correct output files are created", TEST_GROUP) {
    using Catch::Matchers::Contains;

    auto tmp_dir = fs::temp_directory_path() / "dorado_demuxer";

    {
        // Creating local scope for the pipeline because on windows
        // the temporary directory is still being considered open unless
        // the pipeline object is closed. This needs to be looked at.
        // TODO: Address open file issue on windows.
        dorado::PipelineDescriptor pipeline_desc;
        auto demuxer =
                pipeline_desc.add_node<BarcodeDemuxerNode>({}, tmp_dir.string(), 8, false, nullptr);

        auto pipeline = dorado::Pipeline::create(std::move(pipeline_desc), nullptr);

        SamHdrPtr hdr(sam_hdr_init());
        sam_hdr_add_line(hdr.get(), "SQ", "ID", "foo", "LN", "100", "SN", "ref", NULL);

        auto& demux_writer_ref = dynamic_cast<BarcodeDemuxerNode&>(pipeline->get_node_ref(demuxer));
        demux_writer_ref.set_header(hdr.get());

        for (auto bc : {"bc01", "bc02", "bc03"}) {
            auto records = create_bam_reader(bc);
            for (auto& rec : records) {
                pipeline->push_message(std::move(rec));
            }
        }

        pipeline->terminate(DefaultFlushOptions());

        const std::unordered_set<std::string> expected_files = {"bc01.bam", "bc02.bam", "bc03.bam"};

        for (const auto& entry : fs::directory_iterator(tmp_dir)) {
            CHECK(expected_files.find(entry.path().filename().string()) != expected_files.end());
        }
    }

    fs::remove_all(tmp_dir);
}

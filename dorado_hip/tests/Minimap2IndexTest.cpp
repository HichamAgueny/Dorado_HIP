#include "alignment/Minimap2Index.h"

#include "StreamUtils.h"
#include "TestUtils.h"

#include <catch2/catch.hpp>

#include <filesystem>

#define TEST_GROUP "[alignment::Minimap2Index]"

using namespace ont::test_utils::streams;

namespace {

class Minimap2IndexTestFixture {
protected:
    dorado::alignment::Minimap2Index cut{};
    std::string reference_file;

public:
    Minimap2IndexTestFixture() {
        const std::string read_id{"aligner_node_test"};
        std::filesystem::path aligner_test_dir{get_aligner_data_dir()};
        auto ref = aligner_test_dir / "target.fq";
        reference_file = ref.string();

        cut.initialise(dorado::alignment::dflt_options);
    }
};

}  // namespace

namespace dorado::alignment::test {

TEST_CASE(TEST_GROUP " initialise() with default options does not throw", TEST_GROUP) {
    Minimap2Index cut{};

    REQUIRE_NOTHROW(cut.initialise(dflt_options));
}

TEST_CASE(TEST_GROUP " initialise() with default options returns true", TEST_GROUP) {
    Minimap2Index cut{};

    REQUIRE(cut.initialise(dflt_options));
}

TEST_CASE(TEST_GROUP " initialise() with default options sets indexing options", TEST_GROUP) {
    Minimap2Index cut{};

    cut.initialise(dflt_options);

    CHECK(cut.index_options().k == dflt_options.kmer_size);
    CHECK(cut.index_options().w == dflt_options.window_size);
    CHECK(cut.index_options().batch_size == dflt_options.index_batch_size);
}

TEST_CASE(TEST_GROUP " initialise() with default options sets mapping options", TEST_GROUP) {
    Minimap2Index cut{};

    cut.initialise(dflt_options);

    CHECK(cut.mapping_options().bw == dflt_options.bandwidth);
    CHECK(cut.mapping_options().bw_long == dflt_options.bandwidth_long);
    CHECK(cut.mapping_options().best_n == dflt_options.best_n_secondary);
    CHECK(cut.mapping_options().flag > 0);  // Just checking it's been updated.
}

TEST_CASE(TEST_GROUP " initialise() with invalid options returns false", TEST_GROUP) {
    Minimap2Index cut{};
    auto invalid_options{dflt_options};
    invalid_options.bandwidth = invalid_options.bandwidth_long + 1;

    bool result{};
    SuppressStderr::invoke(
            [&result, &invalid_options, &cut] { result = cut.initialise(invalid_options); });

    REQUIRE_FALSE(result);
}

TEST_CASE_METHOD(Minimap2IndexTestFixture,
                 TEST_GROUP " load() with invalid reference file returns reference_file_not_found",
                 TEST_GROUP) {
    REQUIRE(cut.load("some_reference_file", 1) == IndexLoadResult::reference_file_not_found);
}

TEST_CASE_METHOD(Minimap2IndexTestFixture,
                 TEST_GROUP " load() with valid reference file returns success",
                 TEST_GROUP) {
    REQUIRE(cut.load(reference_file, 1) == IndexLoadResult::success);
}

TEST_CASE_METHOD(Minimap2IndexTestFixture,
                 TEST_GROUP " create_compatible_index() with invalid mapping options returns null",
                 TEST_GROUP) {
    cut.load(reference_file, 1);
    Minimap2Options invalid_compatible_options{dflt_options};
    invalid_compatible_options.bandwidth = invalid_compatible_options.bandwidth_long + 1;

    std::shared_ptr<Minimap2Index> compatible_index{};
    {
        SuppressStderr suppressed{};
        compatible_index = cut.create_compatible_index(invalid_compatible_options);
    }

    REQUIRE(compatible_index == nullptr);
}

TEST_CASE_METHOD(Minimap2IndexTestFixture,
                 TEST_GROUP " create_compatible_index() with valid options returns non-null",
                 TEST_GROUP) {
    cut.load(reference_file, 1);
    Minimap2Options compatible_options{dflt_options};
    ++compatible_options.best_n_secondary;

    REQUIRE(cut.create_compatible_index(compatible_options) != nullptr);
}

TEST_CASE_METHOD(Minimap2IndexTestFixture,
                 TEST_GROUP
                 " create_compatible_index() with valid options returns Minimap2Index with same "
                 "underlying index",
                 TEST_GROUP) {
    cut.load(reference_file, 1);
    Minimap2Options compatible_options{dflt_options};
    ++compatible_options.best_n_secondary;

    auto compatible_index = cut.create_compatible_index(compatible_options);

    REQUIRE(compatible_index->index() == cut.index());
}

TEST_CASE_METHOD(Minimap2IndexTestFixture,
                 TEST_GROUP
                 " create_compatible_index() with valid options returns Minimap2Index with mapping "
                 "options updated",
                 TEST_GROUP) {
    cut.load(reference_file, 1);
    Minimap2Options compatible_options{dflt_options};
    ++compatible_options.best_n_secondary;

    auto compatible_index = cut.create_compatible_index(compatible_options);

    REQUIRE(compatible_index->mapping_options().best_n == dflt_options.best_n_secondary + 1);
}

}  // namespace dorado::alignment::test

#include "Version.h"
#include "cli/cli_utils.h"
#include "read_pipeline/AdapterDetectorNode.h"
#include "read_pipeline/HtsReader.h"
#include "read_pipeline/HtsWriter.h"
#include "read_pipeline/ProgressTracker.h"
#include "utils/basecaller_utils.h"
#include "utils/log_utils.h"
#include "utils/stats.h"

#include <spdlog/spdlog.h>

#include <chrono>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <vector>
using namespace std::chrono_literals;

#ifndef _WIN32
#include <unistd.h>
#endif

namespace dorado {

namespace {

void add_pg_hdr(sam_hdr_t* hdr) {
    sam_hdr_add_line(hdr, "PG", "ID", "trim", "PN", "dorado", "VN", DORADO_VERSION, NULL);
}

}  // anonymous namespace

int trim(int argc, char* argv[]) {
    utils::InitLogging();

    argparse::ArgumentParser parser("dorado", DORADO_VERSION, argparse::default_arguments::help);
    parser.add_description("Adapter/primer trimming tool.");
    parser.add_argument("reads")
            .help("Path to a file with reads to trim. Can be in any HTS format.")
            .nargs(argparse::nargs_pattern::any);
    parser.add_argument("-t", "--threads")
            .help("Combined number of threads for adapter/primer detection and output generation. "
                  "Default uses "
                  "all available threads.")
            .default_value(0)
            .scan<'i', int>();
    parser.add_argument("-n", "--max-reads")
            .help("Maximum number of reads to process. Mainly for debugging. Process all reads by "
                  "default.")
            .default_value(0)
            .scan<'i', int>();
    parser.add_argument("-l", "--read-ids")
            .help("A file with a newline-delimited list of reads to trim.")
            .default_value(std::string(""));
    int verbosity = 0;
    parser.add_argument("-v", "--verbose")
            .default_value(false)
            .implicit_value(true)
            .nargs(0)
            .action([&](const auto&) { ++verbosity; })
            .append();
    parser.add_argument("--emit-fastq")
            .help("Output in fastq format. Default is BAM.")
            .default_value(false)
            .implicit_value(true);
    parser.add_argument("--no-trim-primers")
            .help("Skip primer detection and trimming. Only adapters will be detected and trimmed.")
            .default_value(false)
            .implicit_value(true);

    try {
        parser.parse_args(argc, argv);
    } catch (const std::exception& e) {
        std::ostringstream parser_stream;
        parser_stream << parser;
        spdlog::error("{}\n{}", e.what(), parser_stream.str());
        std::exit(EXIT_FAILURE);
    }

    if (parser.get<bool>("--verbose")) {
        utils::SetVerboseLogging(static_cast<dorado::utils::VerboseLogLevel>(verbosity));
    }

    auto reads(parser.get<std::vector<std::string>>("reads"));
    auto threads(parser.get<int>("threads"));
    auto max_reads(parser.get<int>("max-reads"));

    threads = threads == 0 ? std::thread::hardware_concurrency() : threads;
    // The input thread is the total number of threads to use for dorado
    // adapter/primer trimming. Heuristically use 10% of threads for BAM
    // generation and rest for trimming.
    auto [trim_threads, trim_writer_threads] =
            cli::worker_vs_writer_thread_allocation(threads, 0.1f);
    spdlog::debug("> adapter/primer trimming threads {}, writer threads {}", trim_threads,
                  trim_writer_threads);

    auto read_list = utils::load_read_list(parser.get<std::string>("--read-ids"));

    if (reads.empty()) {
#ifndef _WIN32
        if (isatty(fileno(stdin))) {
            std::cout << parser << std::endl;
            std::exit(EXIT_FAILURE);
        }
#endif
        reads.push_back("-");
    } else if (reads.size() > 1) {
        spdlog::error("> multi file input not yet handled");
        std::exit(EXIT_FAILURE);
    }

    HtsReader reader(reads[0], read_list);
    auto header = SamHdrPtr(sam_hdr_dup(reader.header));
    add_pg_hdr(header.get());

    auto output_mode = HtsWriter::OutputMode::BAM;

    auto emit_fastq = parser.get<bool>("--emit-fastq");
    auto emit_sam = !emit_fastq;

    if (emit_fastq) {
        spdlog::info(" - Note: FASTQ output is not recommended as not all data can be preserved.");
        output_mode = HtsWriter::OutputMode::FASTQ;
    } else if (emit_sam || utils::is_fd_tty(stdout)) {
        output_mode = HtsWriter::OutputMode::SAM;
    } else if (utils::is_fd_pipe(stdout)) {
        output_mode = HtsWriter::OutputMode::UBAM;
    }

    PipelineDescriptor pipeline_desc;
    auto hts_writer = pipeline_desc.add_node<HtsWriter>({}, "-", output_mode, trim_writer_threads);

    pipeline_desc.add_node<AdapterDetectorNode>({hts_writer}, trim_threads, true,
                                                !parser.get<bool>("--no-trim-primers"));

    // Create the Pipeline from our description.
    std::vector<dorado::stats::StatsReporter> stats_reporters;
    auto pipeline = Pipeline::create(std::move(pipeline_desc), &stats_reporters);
    if (pipeline == nullptr) {
        spdlog::error("Failed to create pipeline");
        std::exit(EXIT_FAILURE);
    }

    // At present, header output file header writing relies on direct node method calls
    // rather than the pipeline framework.
    auto& hts_writer_ref = dynamic_cast<HtsWriter&>(pipeline->get_node_ref(hts_writer));
    hts_writer_ref.set_and_write_header(header.get());

    // Set up stats counting
    std::vector<dorado::stats::StatsCallable> stats_callables;
    ProgressTracker tracker(0, false);
    stats_callables.push_back(
            [&tracker](const stats::NamedStats& stats) { tracker.update_progress_bar(stats); });
    constexpr auto kStatsPeriod = 100ms;
    auto stats_sampler = std::make_unique<dorado::stats::StatsSampler>(
            kStatsPeriod, stats_reporters, stats_callables, static_cast<size_t>(0));
    // End stats counting setup.

    spdlog::info("> starting adapter/primer trimming");
    reader.read(*pipeline, max_reads);

    // Wait for the pipeline to complete.  When it does, we collect
    // final stats to allow accurate summarisation.
    auto final_stats = pipeline->terminate(DefaultFlushOptions());

    stats_sampler->terminate();

    tracker.update_progress_bar(final_stats);
    tracker.summarize();

    spdlog::info("> finished adapter/primer trimming");

    return 0;
}

}  // namespace dorado

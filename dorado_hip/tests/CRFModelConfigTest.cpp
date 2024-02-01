#include "basecall/CRFModelConfig.h"

#include "TestUtils.h"

#include <catch2/catch.hpp>
#include <torch/torch.h>

#include <cstdlib>
#include <filesystem>
#include <random>
#include <string>

#define CUT_TAG "[CRFModelConfig]"

using namespace dorado::basecall;
namespace fs = std::filesystem;

TEST_CASE(CUT_TAG ": test dna_r9.4.1 hac@v3.3 model load", CUT_TAG) {
    const fs::path path = fs::path(get_data_dir("model_configs/dna_r9.4.1_e8_hac@v3.3"));
    const CRFModelConfig config = load_crf_model_config(path);

    CHECK(config.model_path == path);
    CHECK(config.bias == true);
    CHECK(config.num_features == 1);
    CHECK(config.stride == 5);
    CHECK(config.lstm_size == 384);
    CHECK(config.blank_score == 2.0f);
    CHECK(config.scale == 5.0f);
    CHECK(config.state_len == 4);
    CHECK(config.outsize == 1024);
    CHECK(config.clamp == false);
    CHECK(config.out_features.has_value() == false);
    CHECK(config.sample_type == SampleType::DNA);

    CHECK(config.qbias == -0.1721f);
    CHECK(config.qscale == 0.9356f);
    CHECK(config.sample_rate == -1);

    SignalNormalisationParams sig;
    // r9.4.1 model does not use quantile scaling
    sig.strategy = ScalingStrategy::MED_MAD;
    CHECK(config.signal_norm_params.strategy == sig.strategy);

    const QuantileScalingParams &qsp = config.signal_norm_params.quantile;
    CHECK(qsp.quantile_a == sig.quantile.quantile_a);
    CHECK(qsp.quantile_b == sig.quantile.quantile_b);
    CHECK(qsp.scale_multiplier == sig.quantile.scale_multiplier);
    CHECK(qsp.shift_multiplier == sig.quantile.shift_multiplier);

    const StandardisationScalingParams &ssp = config.signal_norm_params.standarisation;
    CHECK(ssp.standardise == false);
    CHECK(ssp.standardise == sig.standarisation.standardise);
    CHECK(ssp.mean == sig.standarisation.mean);
    CHECK(ssp.stdev == sig.standarisation.stdev);

    ConvParams conv1 = config.convs[0];
    CHECK(conv1.activation == Activation::SWISH);
    CHECK(conv1.insize == 1);
    CHECK(conv1.size == 4);  // default first_conv value
    CHECK(conv1.stride == 1);
    CHECK(conv1.winlen == 5);

    ConvParams conv2 = config.convs[1];
    CHECK(conv2.activation == Activation::SWISH);
    CHECK(conv2.insize == 4);
    CHECK(conv2.size == 16);
    CHECK(conv2.stride == 1);
    CHECK(conv2.winlen == 5);

    ConvParams conv3 = config.convs[2];
    CHECK(conv3.activation == Activation::SWISH);
    CHECK(conv3.insize == 16);
    CHECK(conv3.size == 384);
    CHECK(conv3.stride == 5);
    CHECK(conv3.winlen == 19);
}

TEST_CASE(CUT_TAG ": test dna_r10.4.1 fast@v4.0.0 model load", CUT_TAG) {
    const fs::path path =
            fs::path(get_data_dir("model_configs/dna_r10.4.1_e8.2_260bps_fast@v4.0.0"));
    const CRFModelConfig config = load_crf_model_config(path);

    CHECK(config.model_path == path);
    CHECK(config.bias == false);
    CHECK(config.num_features == 1);
    CHECK(config.stride == 5);
    CHECK(config.lstm_size == 96);
    CHECK(config.blank_score == 2.0f);
    CHECK(config.scale == 1.0f);
    CHECK(config.state_len == 3);
    CHECK(config.outsize == 256);
    CHECK(config.clamp == false);
    CHECK(config.out_features.has_value() == false);
    CHECK(config.sample_type == SampleType::DNA);

    CHECK(config.qbias == -3.0f);
    CHECK(config.qscale == 1.04f);
    CHECK(config.sample_rate == -1);

    SignalNormalisationParams sig;
    // r9.4.1 model does not use quantile scaling
    sig.strategy = ScalingStrategy::QUANTILE;
    CHECK(config.signal_norm_params.strategy == sig.strategy);

    const QuantileScalingParams &qsp = config.signal_norm_params.quantile;
    CHECK(qsp.quantile_a == sig.quantile.quantile_a);
    CHECK(qsp.quantile_b == sig.quantile.quantile_b);
    CHECK(qsp.scale_multiplier == sig.quantile.scale_multiplier);
    CHECK(qsp.shift_multiplier == sig.quantile.shift_multiplier);

    const StandardisationScalingParams &ssp = config.signal_norm_params.standarisation;
    CHECK(ssp.standardise == false);
    CHECK(ssp.standardise == sig.standarisation.standardise);
    CHECK(ssp.mean == sig.standarisation.mean);
    CHECK(ssp.stdev == sig.standarisation.stdev);

    ConvParams conv1 = config.convs[0];
    CHECK(conv1.activation == Activation::SWISH);
    CHECK(conv1.insize == 1);
    CHECK(conv1.size == 16);  // default first_conv value
    CHECK(conv1.stride == 1);
    CHECK(conv1.winlen == 5);

    ConvParams conv2 = config.convs[1];
    CHECK(conv2.activation == Activation::SWISH);
    CHECK(conv2.insize == 16);
    CHECK(conv2.size == 16);
    CHECK(conv2.stride == 1);
    CHECK(conv2.winlen == 5);

    ConvParams conv3 = config.convs[2];
    CHECK(conv3.activation == Activation::SWISH);
    CHECK(conv3.insize == 16);
    CHECK(conv3.size == 96);
    CHECK(conv3.stride == 5);
    CHECK(conv3.winlen == 19);
}

TEST_CASE(CUT_TAG ": test dna_r10.4.1 hac@v4.2.0 model load", CUT_TAG) {
    const fs::path path =
            fs::path(get_data_dir("model_configs/dna_r10.4.1_e8.2_400bps_hac@v4.2.0"));
    const CRFModelConfig config = load_crf_model_config(path);

    CHECK(config.model_path == path);
    CHECK(config.bias == true);
    CHECK(config.num_features == 1);
    CHECK(config.stride == 6);
    CHECK(config.lstm_size == 384);
    CHECK(config.blank_score == 2.0f);
    CHECK(config.scale == 1.0f);
    CHECK(config.state_len == 4);
    CHECK(config.outsize == 1024);
    CHECK(config.clamp == true);
    CHECK(config.out_features.has_value());
    CHECK(config.out_features.value() == 128);
    CHECK(config.sample_type == SampleType::DNA);

    CHECK(config.qbias == -0.2f);
    CHECK(config.qscale == 0.95f);
    CHECK(config.sample_rate == 5000);

    SignalNormalisationParams sig;
    sig.strategy = ScalingStrategy::QUANTILE;
    CHECK(config.signal_norm_params.strategy == sig.strategy);

    const QuantileScalingParams &qsp = config.signal_norm_params.quantile;
    CHECK(qsp.quantile_a == sig.quantile.quantile_a);
    CHECK(qsp.quantile_b == sig.quantile.quantile_b);
    CHECK(qsp.scale_multiplier == sig.quantile.scale_multiplier);
    CHECK(qsp.shift_multiplier == sig.quantile.shift_multiplier);

    const StandardisationScalingParams &ssp = config.signal_norm_params.standarisation;
    CHECK(ssp.standardise == false);
    CHECK(ssp.standardise == sig.standarisation.standardise);
    CHECK(ssp.mean == sig.standarisation.mean);
    CHECK(ssp.stdev == sig.standarisation.stdev);

    ConvParams conv1 = config.convs[0];
    CHECK(conv1.activation == Activation::SWISH_CLAMP);
    CHECK(conv1.insize == 1);
    CHECK(conv1.size == 16);
    CHECK(conv1.stride == 1);
    CHECK(conv1.winlen == 5);

    ConvParams conv2 = config.convs[1];
    CHECK(conv2.activation == Activation::SWISH_CLAMP);
    CHECK(conv2.insize == 16);
    CHECK(conv2.size == 16);
    CHECK(conv2.stride == 1);
    CHECK(conv2.winlen == 5);

    ConvParams conv3 = config.convs[2];
    CHECK(conv3.activation == Activation::SWISH_CLAMP);
    CHECK(conv3.insize == 16);
    CHECK(conv3.size == 384);
    CHECK(conv3.stride == 6);
    CHECK(conv3.winlen == 19);
}

TEST_CASE(CUT_TAG ": test dna_r10.4.1 hac@v4.3.0 pa model load", CUT_TAG) {
    const fs::path path =
            fs::path(get_data_dir("model_configs/dna_r10.4.1_e8.2_400bps_hac@v4.3.0"));
    const CRFModelConfig config = load_crf_model_config(path);

    CHECK(config.model_path == path);
    CHECK(config.bias == false);
    CHECK(config.num_features == 1);
    CHECK(config.stride == 6);
    CHECK(config.lstm_size == 384);
    CHECK(config.blank_score == 2.0f);
    CHECK(config.scale == 1.0f);
    CHECK(config.state_len == 4);
    CHECK(config.outsize == 1024);
    CHECK(config.clamp == true);
    CHECK(config.out_features.has_value() == false);
    CHECK(config.sample_type == SampleType::DNA);

    CHECK(config.qbias == -1.1f);
    CHECK(config.qscale == 1.1f);
    CHECK(config.sample_rate == 5000);

    SignalNormalisationParams sig;
    sig.strategy = ScalingStrategy::PA;
    CHECK(config.signal_norm_params.strategy == sig.strategy);

    const QuantileScalingParams &qsp = config.signal_norm_params.quantile;
    CHECK(qsp.quantile_a == sig.quantile.quantile_a);
    CHECK(qsp.quantile_b == sig.quantile.quantile_b);
    CHECK(qsp.scale_multiplier == sig.quantile.scale_multiplier);
    CHECK(qsp.shift_multiplier == sig.quantile.shift_multiplier);

    const StandardisationScalingParams &ssp = config.signal_norm_params.standarisation;
    CHECK(ssp.standardise == true);
    CHECK(ssp.mean == 91.88f);
    CHECK(ssp.stdev == 22.65f);

    ConvParams conv1 = config.convs[0];
    CHECK(conv1.activation == Activation::SWISH);
    CHECK(conv1.insize == 1);
    CHECK(conv1.size == 16);
    CHECK(conv1.stride == 1);
    CHECK(conv1.winlen == 5);

    ConvParams conv2 = config.convs[1];
    CHECK(conv2.activation == Activation::SWISH);
    CHECK(conv2.insize == 16);
    CHECK(conv2.size == 16);
    CHECK(conv2.stride == 1);
    CHECK(conv2.winlen == 5);

    ConvParams conv3 = config.convs[2];
    CHECK(conv3.activation == Activation::TANH);
    CHECK(conv3.insize == 16);
    CHECK(conv3.size == 384);
    CHECK(conv3.stride == 6);
    CHECK(conv3.winlen == 19);
}

TEST_CASE(CUT_TAG ": test dna_r10.4.1 hac@v4.3.0 quantile model load", CUT_TAG) {
    const fs::path path =
            fs::path(get_data_dir("model_configs/dna_r10.4.1_e8.2_400bps_hac@v4.3.0_quantile"));
    const CRFModelConfig config = load_crf_model_config(path);

    CHECK(config.model_path == path);
    CHECK(config.bias == false);
    CHECK(config.num_features == 1);
    CHECK(config.stride == 6);
    CHECK(config.lstm_size == 384);
    CHECK(config.blank_score == 2.0f);
    CHECK(config.scale == 1.0f);
    CHECK(config.state_len == 4);
    CHECK(config.outsize == 1024);
    CHECK(config.clamp == true);
    CHECK(config.out_features.has_value() == false);
    CHECK(config.sample_type == SampleType::DNA);

    CHECK(config.qbias == 0.0f);
    CHECK(config.qscale == 1.0f);
    CHECK(config.sample_rate == -1);

    SignalNormalisationParams sig;
    sig.strategy = ScalingStrategy::QUANTILE;
    CHECK(config.signal_norm_params.strategy == sig.strategy);

    const QuantileScalingParams &qsp = config.signal_norm_params.quantile;
    CHECK(qsp.quantile_a == sig.quantile.quantile_a);
    CHECK(qsp.quantile_b == sig.quantile.quantile_b);
    CHECK(qsp.scale_multiplier == sig.quantile.scale_multiplier);
    CHECK(qsp.shift_multiplier == sig.quantile.shift_multiplier);

    const StandardisationScalingParams &ssp = config.signal_norm_params.standarisation;
    CHECK(ssp.standardise == false);
    CHECK(ssp.standardise == sig.standarisation.standardise);
    CHECK(ssp.mean == sig.standarisation.mean);
    CHECK(ssp.stdev == sig.standarisation.stdev);

    ConvParams conv1 = config.convs[0];
    CHECK(conv1.activation == Activation::SWISH);
    CHECK(conv1.insize == 1);
    CHECK(conv1.size == 16);
    CHECK(conv1.stride == 1);
    CHECK(conv1.winlen == 5);

    ConvParams conv2 = config.convs[1];
    CHECK(conv2.activation == Activation::SWISH);
    CHECK(conv2.insize == 16);
    CHECK(conv2.size == 16);
    CHECK(conv2.stride == 1);
    CHECK(conv2.winlen == 5);

    ConvParams conv3 = config.convs[2];
    CHECK(conv3.activation == Activation::TANH);
    CHECK(conv3.insize == 16);
    CHECK(conv3.size == 384);
    CHECK(conv3.stride == 6);
    CHECK(conv3.winlen == 19);
}

TEST_CASE(CUT_TAG ": test rna002 fast@v3 model load", CUT_TAG) {
    const fs::path path = fs::path(get_data_dir("model_configs/rna002_70bps_fast@v3"));
    const CRFModelConfig config = load_crf_model_config(path);

    CHECK(config.model_path == path);
    CHECK(config.bias == true);
    CHECK(config.num_features == 1);
    CHECK(config.stride == 5);
    CHECK(config.lstm_size == 96);
    CHECK(config.blank_score == 2.0f);
    CHECK(config.scale == 5.0f);
    CHECK(config.state_len == 3);
    CHECK(config.outsize == 256);
    CHECK(config.clamp == false);
    CHECK(config.out_features.has_value() == false);
    CHECK(config.sample_type == SampleType::RNA002);

    CHECK(config.qbias == -1.6f);
    CHECK(config.qscale == 1.0f);
    CHECK(config.sample_rate == 3000);

    SignalNormalisationParams sig;
    sig.strategy = ScalingStrategy::QUANTILE;
    CHECK(config.signal_norm_params.strategy == sig.strategy);

    const QuantileScalingParams &qsp = config.signal_norm_params.quantile;
    CHECK(qsp.quantile_a == 0.2f);
    CHECK(qsp.quantile_b == 0.8f);
    CHECK(qsp.scale_multiplier == 0.59f);
    CHECK(qsp.shift_multiplier == 0.48f);

    const StandardisationScalingParams &ssp = config.signal_norm_params.standarisation;
    CHECK(ssp.standardise == false);
    CHECK(ssp.standardise == sig.standarisation.standardise);
    CHECK(ssp.mean == sig.standarisation.mean);
    CHECK(ssp.stdev == sig.standarisation.stdev);

    ConvParams conv1 = config.convs[0];
    CHECK(conv1.activation == Activation::SWISH);
    CHECK(conv1.insize == 1);
    CHECK(conv1.size == 4);  // default first_conv value
    CHECK(conv1.stride == 1);
    CHECK(conv1.winlen == 5);

    ConvParams conv2 = config.convs[1];
    CHECK(conv2.activation == Activation::SWISH);
    CHECK(conv2.insize == 4);
    CHECK(conv2.size == 16);
    CHECK(conv2.stride == 1);
    CHECK(conv2.winlen == 5);

    ConvParams conv3 = config.convs[2];
    CHECK(conv3.activation == Activation::SWISH);
    CHECK(conv3.insize == 16);
    CHECK(conv3.size == 96);
    CHECK(conv3.stride == 5);
    CHECK(conv3.winlen == 19);
}

TEST_CASE(CUT_TAG ": test rna004 sup@v3.0.1 model load", CUT_TAG) {
    const fs::path path = fs::path(get_data_dir("model_configs/rna004_130bps_sup@v3.0.1"));
    const CRFModelConfig config = load_crf_model_config(path);

    CHECK(config.model_path == path);
    CHECK(config.bias == true);
    CHECK(config.num_features == 1);
    CHECK(config.stride == 5);
    CHECK(config.lstm_size == 768);
    CHECK(config.blank_score == 2.0f);
    CHECK(config.scale == 5.0f);
    CHECK(config.state_len == 5);
    CHECK(config.outsize == 4096);
    CHECK(config.clamp == false);
    CHECK(config.out_features.has_value() == false);
    CHECK(config.sample_type == SampleType::RNA004);

    CHECK(config.qbias == -0.1f);
    CHECK(config.qscale == 0.9f);
    CHECK(config.sample_rate == 4000);

    SignalNormalisationParams sig;
    sig.strategy = ScalingStrategy::QUANTILE;
    CHECK(config.signal_norm_params.strategy == ScalingStrategy::QUANTILE);
    // Intentionally edited values from default values to test parser

    const QuantileScalingParams &qsp = config.signal_norm_params.quantile;
    CHECK(qsp.quantile_a == 0.22f);
    CHECK(qsp.quantile_b == 0.88f);
    CHECK(qsp.scale_multiplier == 0.595f);
    CHECK(qsp.shift_multiplier == 0.485f);

    const StandardisationScalingParams &ssp = config.signal_norm_params.standarisation;
    CHECK(ssp.standardise == false);
    CHECK(ssp.standardise == sig.standarisation.standardise);
    CHECK(ssp.mean == sig.standarisation.mean);
    CHECK(ssp.stdev == sig.standarisation.stdev);

    ConvParams conv1 = config.convs[0];
    CHECK(conv1.activation == Activation::SWISH);
    CHECK(conv1.insize == 1);
    CHECK(conv1.size == 4);  // default first_conv value
    CHECK(conv1.stride == 1);
    CHECK(conv1.winlen == 5);

    ConvParams conv2 = config.convs[1];
    CHECK(conv2.activation == Activation::SWISH);
    CHECK(conv2.insize == 4);
    CHECK(conv2.size == 16);
    CHECK(conv2.stride == 1);
    CHECK(conv2.winlen == 5);

    ConvParams conv3 = config.convs[2];
    CHECK(conv3.activation == Activation::SWISH);
    CHECK(conv3.insize == 16);
    CHECK(conv3.size == 768);
    CHECK(conv3.stride == 5);
    CHECK(conv3.winlen == 19);
}

#include "metadata.h"

#include "utils/string_utils.h"

#include <algorithm>
#include <stdexcept>
#include <string>

namespace dorado::models {

namespace model_variant {
const std::unordered_map<std::string, ModelVariant> map = {
        {"auto", ModelVariant::AUTO},
        {"fast", ModelVariant::FAST},
        {"hac", ModelVariant::HAC},
        {"sup", ModelVariant::SUP},
};
}

namespace mods {
const std::unordered_map<std::string, ModsVariant> map = {
        {"5mC_5hmC", ModsVariant::M_5mC_5hmC},
        {"5mCG", ModsVariant::M_5mCG},
        {"5mCG_5hmCG", ModsVariant::M_5mCG_5hmCG},
        {"5mC", ModsVariant::M_5mC},
        {"6mA", ModsVariant::M_6mA},
        {"m6A_DRACH", ModsVariant::M_m6A_DRACH},
};
}

namespace version {
const std::unordered_map<std::string, ModelVersion> map = {
        {"v0.1.0", ModelVersion::v0_1_0}, {"v1.0.0", ModelVersion::v1_0_0},
        {"v1.1.0", ModelVersion::v1_1_0}, {"v1.2.0", ModelVersion::v1_2_0},
        {"v2.0.0", ModelVersion::v2_0_0}, {"v3.0.0", ModelVersion::v3_0_0},
        {"v3.0.1", ModelVersion::v3_0_1}, {"v3.1.0", ModelVersion::v3_1_0},
        {"v3.3.0", ModelVersion::v3_3_0}, {"v3.4.0", ModelVersion::v3_4_0},
        {"v3.5.0", ModelVersion::v3_5_0}, {"v3.5.2", ModelVersion::v3_5_2},
        {"v3.6.0", ModelVersion::v3_6_0}, {"v4.0.0", ModelVersion::v4_0_0},
        {"v4.1.0", ModelVersion::v4_1_0}, {"v4.2.0", ModelVersion::v4_2_0},
        {"v4.3.0", ModelVersion::v4_3_0}, {"latest", ModelVersion::NONE}};
}  // namespace version

const std::unordered_map<std::string, ModelVariant>& model_variants_map() {
    return model_variant::map;
}
const std::unordered_map<std::string, ModsVariant>& mods_variants_map() { return mods::map; }
const std::unordered_map<std::string, ModelVersion>& version_map() { return version::map; }

namespace {

template <typename Variant>
Variant get_variant(const std::string& variant,
                    const std::unordered_map<std::string, Variant>& variants) {
    auto it = variants.find(variant);
    if (it == std::end(variants)) {
        return Variant::NONE;
    }
    return it->second;
}

template <typename Variant>
std::string to_string(const Variant& variant,
                      const std::string& description,
                      const std::unordered_map<std::string, Variant>& variants) {
    auto it = std::find_if(std::begin(variants), std::end(variants),
                           [&variant](auto&& kv) { return kv.second == variant; });

    if (it == std::end(variants)) {
        throw std::logic_error("Unknown + " + description +
                               " enum: " + std::to_string(static_cast<int>(variant)));
    }
    return it->first;
}

}  // namespace

ModelVariant get_model_variant(const std::string& variant) {
    return get_variant(variant, model_variants_map());
}
ModsVariant get_mods_variant(const std::string& variant) {
    return get_variant(variant, mods_variants_map());
}

std::string to_string(const ModelVariant& variant) {
    return to_string(variant, "model variant", model_variants_map());
}

std::string to_string(const ModsVariant& variant) {
    return to_string(variant, "modification variant", mods_variants_map());
}

std::string to_string(const std::set<ModelVariant>& variants, const std::string& separator) {
    std::vector<std::string> strings;
    for (const auto& variant : variants) {
        strings.push_back(to_string(variant, "model variant", model_variants_map()));
    }
    return utils::join(strings, separator);
}

std::string to_string(const ModelVersion& version) {
    return to_string(version, "model version", version_map());
}

std::string to_string(const Chemistry& chemistry) {
    const auto& chemistries = chemistry_variants();
    auto it = chemistries.find(chemistry);
    if (it == std::end(chemistries)) {
        throw std::logic_error("Unknown chemistry enum: " +
                               std::to_string(static_cast<int>(chemistry)));
    }
    return it->second;
}

}  // namespace dorado::models
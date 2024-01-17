#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace dorado::utils {

inline std::vector<std::string> split(std::string_view input, char delimiter) {
    std::vector<std::string> result;
    size_t pos;
    while ((pos = input.find(delimiter)) != std::string_view::npos) {
        result.push_back(std::string(input.substr(0, pos)));
        input.remove_prefix(pos + 1);
    }

    result.push_back(std::string(input));
    return result;
}

inline std::string join(const std::vector<std::string>& inputs, const std::string& separator) {
    std::string result;
    for (const auto& item : inputs) {
        if (!result.empty()) {
            result += separator;
        }
        result += item;
    }
    return result;
}

inline bool starts_with(std::string_view str, std::string_view prefix) {
    return str.rfind(prefix, 0) != std::string::npos;
}

inline bool ends_with(std::string_view str, std::string_view suffix) {
    if (str.length() < suffix.length()) {
        return false;
    }
    return str.substr(str.length() - suffix.length()) == suffix;
}

}  // namespace dorado::utils

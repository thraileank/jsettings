#pragma once
#include <unordered_map>
#include <variant>

#include "../param/Param.hpp"

namespace JSettings {
    using ParamVariant_t = std::variant<Param<int>, Param<std::string>>;
    std::unordered_map<std::string, ParamVariant_t> DUMMY_DEFAULT_PARAMETERS {
        { "MY_STRING_PARAM", Param<std::string>(1, "MY_STRING_PARAM", ValueTypes::STRING, "My default value") },
        { "MY_INTEGER_PARAM", Param<int>(2, "MY_INTEGER_PARAM", ValueTypes::INTEGER, 3) }
    };
};
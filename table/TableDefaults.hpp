#pragma once
#include <unordered_map>

#include "../param/Param.hpp"

namespace JSettings {
    /**
     * Pre-defined map of parameters and their default values
     * because there is no option for adding and removing values
     * from Table by design.
     * Provides initial values for Table.
     */
    ParamsMap_t DEFAULT_PARAMETERS {
        { "MY_STRING_PARAM", Param<std::string>("MY_STRING_PARAM", ValueTypes::STRING, "My default value") },
        { "MY_INTEGER_PARAM", Param<int>("MY_INTEGER_PARAM", ValueTypes::INTEGER, 3) }
    };
};
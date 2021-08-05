#pragma once

#include <unordered_map>

namespace JSettings {
    
    /**
     * Types that can be stored in Param
     */
    enum ValueTypes {
        STRING = 0,
        INTEGER,
        IPV4,
        FQDN
    };

    static std::unordered_map<std::string, ValueTypes> stringToValueTypes = {
        { "NUMERIC", ValueTypes::INTEGER },
        { "STRING", ValueTypes::STRING },
        { "IPV4", ValueTypes::IPV4 },
        { "FQDN", ValueTypes::FQDN }
    };

    static std::unordered_map<ValueTypes, std::string> valueTypesToString = {
        { ValueTypes::INTEGER, "NUMERIC" },
        { ValueTypes::STRING, "STRING"},
        { ValueTypes::IPV4, "IPV4" },
        { ValueTypes::FQDN, "FQDN" }
    };
}

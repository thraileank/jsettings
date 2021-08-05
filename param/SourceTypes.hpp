#pragma once

#include <unordered_map>

namespace JSettings {
    
    /**
     * Sources where a value for parameter may come from.
     * Enum integer corresponds to source priority.
     * If a parameter contains values from more than one source
     * the highest priority source wins.
     */
    enum SourceTypes {
        DEFAULT = 0,
        DHCP,
        HTTP,
        USER
    };

    static std::unordered_map<std::string, SourceTypes> stringToSourceTypes = {
        { "DEFAULT", SourceTypes::DEFAULT },
        { "DHCP", SourceTypes::DHCP },
        { "HTTP", SourceTypes::HTTP },
        { "USER", SourceTypes::USER }
    };

    static std::unordered_map<SourceTypes, std::string> sourceTypesToString = {
        { SourceTypes::DEFAULT, "DEFAULT" },
        { SourceTypes::DHCP, "DHCP" },
        { SourceTypes::HTTP, "HTTP" },
        { SourceTypes::USER, "USER"}
    };
}

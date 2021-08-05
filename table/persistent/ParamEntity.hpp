#pragma once

#include <string>
#include <iostream>

namespace JSettings {
    
    /**
     * Data class that is suitable for storing
     * parameter in a persistent storage
     */
    struct ParamEntity {
        int id = -1;
        std::string name;
        std::string type;
        std::string defaultValue;
        std::string dhcpValue;
        std::string httpValue;
        std::string userValue;

        void debugPrint() {
            std::cout << "id: " << id << "\n"
            << "name: " << name << "\n"
            << "type: " << type << "\n"
            << "defaultValue: " << defaultValue << "\n"
            << "dhcpValue: " << dhcpValue << "\n"
            << "httpValue: " << httpValue << "\n"
            << "userValue: " << userValue << "\n" << std::endl;
        }
    };
};

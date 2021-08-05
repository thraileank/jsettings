#pragma once

#include "../../param/Param.hpp"
#include "ParamEntity.hpp"
#include <sstream>

namespace JSettings {
    using ParamVariant_t = std::variant<Param<int>, Param<std::string>>;
    class ParamTypeConverter {
    public:
        static ParamEntity toParamEntity(ParamVariant_t param) {
            auto caller = [](const auto& obj) {
                ParamEntity entity;
                entity.id = obj.id_;
                entity.name = obj.name_;
                entity.type = valueTypesToString[obj.type_];
                for (const auto value : obj.values_) {
                    std::stringstream s;
                    s << value.second;
                    switch (value.first) {
                        case SourceTypes::DEFAULT:
                            entity.defaultValue = s.str();
                            break;
                        case SourceTypes::DHCP:
                            entity.dhcpValue = s.str();
                            break;
                        case SourceTypes::HTTP:
                            entity.httpValue = s.str();
                            break;
                        case SourceTypes::USER:
                            entity.userValue = s.str();
                            break;
                        default:
                            break;
                    }
                }
                return entity;
            };
            return std::visit(caller, param);
        }

        static ParamVariant_t toParamVariant(ParamEntity entity) {
            switch (stringToValueTypes[entity.type]) {
            case ValueTypes::INTEGER:
                {
                    Param<int> param(entity.id, entity.name, stringToValueTypes[entity.type], std::stoi(entity.defaultValue));
                    if (!entity.dhcpValue.empty()) {
                        param.setValue(SourceTypes::DHCP, std::stoi(entity.dhcpValue));
                    }
                    if (!entity.httpValue.empty()) {
                        param.setValue(SourceTypes::HTTP, std::stoi(entity.httpValue));
                    }
                    if (!entity.userValue.empty()) {
                        param.setValue(SourceTypes::USER, std::stoi(entity.userValue));
                    }
                    return param;
                }
            case ValueTypes::STRING:
            case ValueTypes::IPV4:
            case ValueTypes::FQDN:
            default:
                {
                    Param<std::string> param(entity.id, entity.name, stringToValueTypes[entity.type], entity.defaultValue);
                    if (!entity.dhcpValue.empty()) {
                        param.setValue(SourceTypes::DHCP, entity.dhcpValue);
                    }
                    if (!entity.httpValue.empty()) {
                        param.setValue(SourceTypes::HTTP, entity.httpValue);
                    }
                    if (!entity.userValue.empty()) {
                        param.setValue(SourceTypes::USER, entity.userValue);
                    }
                    return param;
                }
            }
        }
    };
}

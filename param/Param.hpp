#pragma once

#include <iostream>
#include <map>
#include <string>
#include <stdexcept>

#include "ValueTypes.hpp"
#include "SourceTypes.hpp"

namespace JSettings {
    //TODO define concept that will restrict possible types
    template<typename T>
    class Param {
        friend class ParamTypeConverter;
    public:
        Param() = default;

        Param(std::string_view name, ValueTypes type, const T& defaultValue) : Param(-1, name, type, defaultValue) {};

        Param(int id, std::string_view name, ValueTypes type, const T& defaultValue)
            : id_(id)
            , name_(name)
            , type_(type) {
                values_[SourceTypes::DEFAULT] = defaultValue;
            }

        Param(int id, std::string_view name, ValueTypes type, const std::map<SourceTypes, T>& values)
            : id_(id)
            , name_(name)
            , type_(type)
            , values_(values) {}

    public:
        T getValue() const {
            return values_.rbegin()->second;
        }

        T getValue(SourceTypes source) const {
            return values_.at(source);
        }

        void setValue(SourceTypes source, const T& value) {
            if (source == SourceTypes::DEFAULT) {
                throw std::invalid_argument("Attempt to update default value!");
            } else {
                values_[source] = value;
            }
        }

        void deleteValue(SourceTypes source) {
            if (source == SourceTypes::DEFAULT) {
                throw std::invalid_argument("Attempt to delete default value!");
            } else {
                values_.erase(source);
            }
        }

        ValueTypes getType() const {
            return type_;
        }

        std::string getName() const {
            return name_;
        }

        void debugPrint() const {
            std::cout << "==========================\n"
                << "SettingsEntity\n"
                << "id: " << id_ << "\n"
                << "name: " << name_ << "\n"
                << "type: " << type_ << "\n"
                << "values:\n";
            for (const auto value : values_) {
                std::cout << value.first << " " << value.second << "\n";
            }
        }

    private:
        int id_ = -1;
        std::string name_;
        ValueTypes type_;
        std::map<SourceTypes, T> values_;
    };
}

#pragma once

#include <unordered_map>
#include <string>
#include <variant>
#include <memory>

#include "../param/Param.hpp"
#include "persistent/sqlite/SQLitePersistentTable.hpp"
#include "../utils/DispatchQueue.hpp"

namespace JSettings {
    using ParamVariant_t = std::variant<Param<int>, Param<std::string>>;
    using ParamsMap_t = std::unordered_map<std::string, ParamVariant_t>;

    /**
     * The highest level class in the hierarchy.
     * Keeps parameters and allows to set and get them
     * with a respect to priority of sources values came from.
     * Performs synchronization between in-memory and persistent tables
     */
    class Table {
    public:
        Table() = default;
        Table(PersistentTable* config) : persistentConfiguration_(config) {};

        void init() {
            parameters_ = persistentConfiguration_->readAll();
            worker_.start();
        }

        template<typename T>
        T getValue(const std::string& name) const {
            return std::get<Param<T>>(parameters_.at(name)).getValue();
        }

        template<typename T>
        T getValue(const std::string& name, SourceTypes source) const {
            return std::get<Param<T>>(parameters_.at(name)).getValue(source);
        }

        template<typename T>
        void setValue(const std::string& name, SourceTypes source, const T& value) {
            Param<T> paramBeforeModification = std::get<Param<T>>(parameters_.at(name));
            // Here std::get returns reference so we change value of parameter inside the table, actually
            Param<T>& actualParameter = std::get<Param<T>>(parameters_.at(name));
            actualParameter.setValue(source, value);

            worker_.dispatch([this, name, &actualParameter, &paramBeforeModification]{
                try {
                    persistentConfiguration_->write(ParamTypeConverter::toParamEntity(actualParameter));
                } catch (...) {
                    parameters_[name] = paramBeforeModification;
                    std::cout << "Writing to persistent configuration failed" << std::endl;
                    throw std::domain_error("Writing to persistent configuration failed");
                }
            });
        }

        void printAll() {
            for (const auto& param : parameters_) {
                auto caller = [](const auto& obj) { obj.debugPrint(); };
                std::visit(caller, param.second);
            }
        }

    private:
        ParamsMap_t parameters_;
        std::unique_ptr<PersistentTable> persistentConfiguration_;
        DispatchQueue worker_;
    };
}

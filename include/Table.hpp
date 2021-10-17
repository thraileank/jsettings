 
#pragma once

#include <unordered_map>
#include <string>
#include <variant>
#include <memory>

#include "../param/Param.hpp"
#include "../table/persistent/sqlite/SQLitePersistentTable.hpp"
#include "../utils/DispatchQueue.hpp"
#include "../table/TableImpl.hpp"

namespace JSettings {
    using ParamVariant_t = std::variant<Param<int>, Param<std::string>>;
    using ParamsMap_t = std::unordered_map<std::string, ParamVariant_t>;

    class Table {
    public:
        Table() = default;
        Table(PersistentTable* config) : tableImpl_(new TableImpl(config)) {};

        void init() {
            tableImpl_->init();
        }

        template<typename T>
        T getValue(const std::string& name) const {
            return tableImpl_->getValue<T>(name);
        }

        template<typename T>
        T getValue(const std::string& name, SourceTypes source) const {
            return tableImpl_->getValue<T>(name, source);
        }

        template<typename T>
        void setValue(const std::string& name, SourceTypes source, const T& value) {
            tableImpl_->setValue(name, source, value);
        }

        void printAll() {
            tableImpl_->printAll();
        }

    private:
        std::unique_ptr<TableImpl> tableImpl_;
    };
}
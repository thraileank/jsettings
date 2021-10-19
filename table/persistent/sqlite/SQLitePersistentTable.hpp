#pragma once

#include "../PersistentTable.hpp"
#include "SQLiteAdapter.hpp"
#include "../ParamTypeConverter.hpp"

namespace JSettings {
    class SQLitePersistentTable : public PersistentTable {
    public:
        SQLitePersistentTable(
            const std::string_view databasePath,
            const std::string_view defaultTableName,
            const std::list<ParamVariant_t> defaults
        ) : database_(databasePath) {
            database_.open();
            database_.createOrUpdateTable(defaultTableName, defaults);
        }

        ~SQLitePersistentTable() {}

        ParamVariant_t read(const std::string_view name) override {
            return ParamTypeConverter::toParamVariant(database_.read("PARAMETERS", name));
        }

        ParamsMap_t readAll() override {
            const auto entities = database_.readAll("PARAMETERS");
            ParamsMap_t params;
            for (const auto& entity : entities) {
                params[entity.name] = ParamTypeConverter::toParamVariant(entity);
            }

            return params;
        }

        void write(const ParamEntity& param) override {
            database_.write("PARAMETERS", param);
        }

    private:
        SQLiteAdapter database_;
    };
};

#pragma once

#include "../PersistentTable.hpp"
#include "SQLiteAdapter.hpp"
#include "../ParamTypeConverter.hpp"
#include "../../TableDefaults.hpp"

namespace JSettings {
    class SQLitePersistentTable : public PersistentTable {
    public:
        SQLitePersistentTable() : database_("parameters.db") {
            database_.open();
            database_.createTableIfNotExists("PARAMETERS", DEFAULT_PARAMETERS);
        }

        SQLitePersistentTable(
            const std::string_view databasePath,
            const std::string_view defaultTableName,
            const ParamsMap_t defaults
        ) : database_(databasePath) {
            database_.open();
            database_.createTableIfNotExists(defaultTableName, defaults);
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

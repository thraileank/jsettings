#pragma once

#include <list>

#include "../../../3rdparty/sqlite3/sqlite3.h"

#include "../ParamTypeConverter.hpp"
#include "SQLiteDAO.hpp"

namespace JSettings {
    using ParamsMap_t = std::unordered_map<std::string, ParamVariant_t>;
    class SQLiteAdapter {
    public:
        SQLiteAdapter() : databasePath_("parameters.db") {};
        SQLiteAdapter(const std::string_view path) : databasePath_(path) {};
        ~SQLiteAdapter() {
            close();
        }

        void open() {
            int rc = sqlite3_open(databasePath_.data(), &database_);
            if (rc == SQLITE_OK) {
                dao_.setDatabase(database_);
            } else {
                //TODO: handle error cases
                throw std::domain_error(std::string("Unable to open SQLite database connection: ") + lastErrorMsg_);
            }
        }

        void createTableIfNotExists(
            const std::string_view tableName,
            const ParamsMap_t& defaults
        ) {
            if (tableExists(tableName)) {
                updateTableIfNeeded(tableName, defaults);
            } else {
                createNewTable(tableName, defaults);
            }
        }

        void write(const std::string_view tableName, const ParamEntity& param) {
            dao_.update(tableName, param);
        }

        ParamEntity read(const std::string_view tableName, const std::string_view name) {
            return dao_.read(tableName, name);
        }

        std::list<ParamEntity> readAll(const std::string_view tableName) {
            return dao_.readAll(tableName);
        }

    private:
        void close() {
            sqlite3_free(lastErrorMsg_); //sqlite3_free performs null check
            lastErrorMsg_ = nullptr;
            int rc = sqlite3_close(database_);
            if (rc != SQLITE_OK) {
                throw std::domain_error("Unable to close SQLite database connection!");
            }
        }

        bool tableExists(const std::string_view tableName) {
            bool exists = false;
            std::stringstream operation;
            operation << "SELECT name FROM sqlite_master WHERE type =\'table\' AND name =\'"<< tableName << "\'";
            sqlite3_stmt* stmt;
            const char* tail;
            // TODO: check if it's need to free tail after the operation
            int rc = sqlite3_prepare_v2(database_, operation.str().c_str(), operation.str().length(), &stmt, &tail);
            rc = sqlite3_step(stmt);
            if (sqlite3_finalize(stmt) == SQLITE_OK) {
                switch (rc) {
                case SQLITE_ROW:
                    exists = true;
                    break;
                case SQLITE_DONE:
                    break;
                default:
                    throw std::domain_error("Error during check if table exists");
                    break;
                }
            } else {
                throw std::domain_error("Couldn't finalize SQL statement");
            }
            return exists;
        }

        void createNewTable(const std::string_view tableName, const ParamsMap_t& defaults) {
            std::stringstream operation;
            operation << "CREATE TABLE " << tableName <<"("
            << "ID INT PRIMARY KEY NOT NULL,"
            << "NAME TEXT NOT NULL,"
            << "TYPE TEXT NOT NULL,"
            << "DFLT TEXT NOT NULL,"
            << "DHCP TEXT,"
            << "HTTP TEXT,"
            << "USER TEXT);";
            int rc = sqlite3_exec(database_, operation.str().c_str(), nullptr, 0, &lastErrorMsg_);
            if (rc == SQLITE_OK) {
                populateTableWithDefaults(tableName, defaults);
            } else {
                std::stringstream error;
                error << "Error during table creation: " << rc << " - " << lastErrorMsg_;
                sqlite3_free(lastErrorMsg_);
                lastErrorMsg_ = nullptr;
                throw std::domain_error(error.str());
            }
        }

        void updateTableIfNeeded(const std::string_view tableName, const ParamsMap_t& defaults) {
            std::unordered_map<std::string, ParamEntity> entitiesForUpdate;
            auto caller = [](auto& obj) {
                return ParamTypeConverter::toParamEntity(obj);
            };
            for (const auto param : defaults) {
                entitiesForUpdate[param.first] = std::visit(caller, param.second);
            }

            auto persistentEntities = readAll(tableName);
            auto paramsInPersistentIter = persistentEntities.begin();
            while (paramsInPersistentIter != persistentEntities.end()) {
                if (entitiesForUpdate.contains(paramsInPersistentIter->name)) {
                    if (entitiesForUpdate[paramsInPersistentIter->name].defaultValue == paramsInPersistentIter->defaultValue) {
                        entitiesForUpdate.erase(paramsInPersistentIter->name);
                    }
                } else {
                    // Some parameters may be removed from a newer version of default parameters
                    dao_.remove(tableName, paramsInPersistentIter->name);
                }
                paramsInPersistentIter++;
            }

            for (const auto entity : entitiesForUpdate) {
                dao_.createOrUpdateDefaultValue(tableName, entity.second);
            }
        }

        void populateTableWithDefaults(const std::string_view tableName, const ParamsMap_t& defaults) {
            auto caller = [](const auto& obj) {
                return ParamTypeConverter::toParamEntity(obj);
            };

            for (const auto& entry : defaults) {
                ParamEntity p = std::visit(
                    caller,
                    entry.second
                );
                dao_.create(tableName, p);
            }
        }

    private:
        const std::string_view databasePath_;
        sqlite3* database_;
        char* lastErrorMsg_ = nullptr;
        SQLiteDAO dao_;
    };
};

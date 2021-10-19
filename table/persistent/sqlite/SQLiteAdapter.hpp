#pragma once

#include <list>

#include "sqlite3/sqlite3.h"

#include "table/persistent/ParamTypeConverter.hpp"
#include "SQLiteDAO.hpp"

namespace JSettings {
    using ParamsMap_t = std::unordered_map<std::string, ParamVariant_t>;

    constexpr int FIRST_PARAM_ID = 1;

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

        void createOrUpdateTable(
            const std::string_view tableName,
            const ParamsMap_t& defaults
        ) {
            if (tableExists(tableName)) {
                updateTableIfNeeded(tableName, defaults);
            } else {
                createTable(tableName, defaults);
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

        void createTable(const std::string_view tableName, const ParamsMap_t& defaults) {
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

            // Convert defaultValues to POD entities first
            std::unordered_map<std::string, ParamEntity> entitiesForUpdate;
            auto caller = [](auto& obj) {
                return ParamTypeConverter::toParamEntity(obj);
            };
            for (const auto param : defaults) {
                entitiesForUpdate[param.first] = std::visit(caller, param.second);
            }

            // Now get existing values from the database and compare them with defaults
            // because set of parameters may be changed during development
            std::list<ParamEntity> persistentEntities = readAll(tableName);
            int maximumExistingId = 0;
            for (const auto& oldEntity : persistentEntities) {
                if (entitiesForUpdate.contains(oldEntity.name)) {
                    if (entitiesForUpdate[oldEntity.name].defaultValue == oldEntity.defaultValue) {
                        entitiesForUpdate.erase(oldEntity.name);
                    } else {
                        entitiesForUpdate[oldEntity.name].id = oldEntity.id;
                    }
                    maximumExistingId = oldEntity.id;
                } else {
                    // We get here if some parameters are removed from a newer version of default parameters
                    // thus we need to remove them from the database
                    dao_.remove(tableName, oldEntity.name);
                }
            }

            for (auto& entity : entitiesForUpdate) {
                if (entity.second.id == -1) {
                    entity.second.id = ++maximumExistingId;
                }
                dao_.createEntryOrUpdateDefaultValue(tableName, entity.second);
            }
        }

        void populateTableWithDefaults(const std::string_view tableName, const ParamsMap_t& defaults) {
            auto caller = [](const auto& obj) {
                return ParamTypeConverter::toParamEntity(obj);
            };

            int entityId = FIRST_PARAM_ID;
            for (const auto& entry : defaults) {
                ParamEntity p = std::visit(
                    caller,
                    entry.second
                );
                p.id = entityId++;
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

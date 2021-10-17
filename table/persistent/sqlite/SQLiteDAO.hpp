 #pragma once
 
#include <sstream>
#include <stdexcept>
#include <string_view>

#include "../../../3rdparty/sqlite3/sqlite3.h"

#include "../ParamEntity.hpp"

namespace JSettings {
    class SQLiteDAO {
    public:
        void setDatabase(sqlite3* database) {
            database_ = database;
        }
        void create(const std::string_view tableName, const ParamEntity& param) {
            // INSERT INTO tableName (ID, NAME, TYPE, DFLT, DHCP, HTTP, USER)
            // VALUES (param.id, 'param.name', 'param.type', 'param.defaultValue',
            // 'param.dhcpValue', 'param.httpValue', 'param.userValue')
            std::stringstream operation;
            operation << "INSERT INTO " << tableName <<
            " (ID, NAME, TYPE, DFLT, DHCP, HTTP, USER) VALUES ("
            << param.id << ", "
            << "\'" << param.name << "\', "
            << "\'" << param.type << "\', "
            << "\'" << param.defaultValue << "\', "
            << "\'" << param.dhcpValue << "\', "
            << "\'" << param.httpValue << "\', "
            << "\'" << param.userValue << "\')";
            int rc = sqlite3_exec(database_, operation.str().c_str(), nullptr, 0, &lastErrorMsg_);
            if (rc != SQLITE_OK) {
                // TODO: operations below are similar across all sqlite error cases.
                // Extract them into the separate method.
                // Or use RAII and wrap lastErrorMsg_ pointer into a class that will
                // be created in the beginning of each method. It will also help to get rid
                // of lastErrorMsg_ member and mark methods as const.
                const std::string errorMsg = "Failed to create entry: " + param.name + ", " + lastErrorMsg_;
                sqlite3_free(lastErrorMsg_);
                lastErrorMsg_ = nullptr;
                throw std::domain_error(errorMsg);
            }
        }

        ParamEntity read(const std::string_view tableName, const std::string_view name) {
            const ParamEntity param;
            std::stringstream operation;
            operation << "SELECT * FROM " << tableName << " WHERE NAME = " << "\'" << name << "\'";
            int rc = sqlite3_exec(
                database_, operation.str().c_str(), SQLiteDAO::readCallback, (void*) &param, &lastErrorMsg_);
            if (rc != SQLITE_OK) {
                const std::string errorMsg =  "Failed to read entry: " + std::string(name) + ", " + lastErrorMsg_;
                sqlite3_free(lastErrorMsg_);
                lastErrorMsg_ = nullptr;
                throw std::domain_error(errorMsg);
            }
            return param;
        }

        std::list<ParamEntity> readAll(const std::string_view tableName) {
            const std::list<ParamEntity> allParams;
            std::stringstream operation;
            operation << "SELECT * FROM " << tableName;
            int rc = sqlite3_exec(
                database_, operation.str().c_str(), SQLiteDAO::readAllCallback, (void*) &allParams, &lastErrorMsg_);
            if (rc != SQLITE_OK) {
                const std::string errorMsg =  "Failed to read all entries: " + std::string(lastErrorMsg_);
                sqlite3_free(lastErrorMsg_);
                lastErrorMsg_ = nullptr;
                throw std::domain_error(errorMsg);
            }
            return allParams;
        }

        void update(const std::string_view tableName, const ParamEntity& param) {
            std::stringstream operation;
            // UPDATE tableName SET DHCP = 'param.dhcpValue',
            // HTTP = 'param.httpValue', USER = 'param.userValue' WHERE ID = param.id
            operation << "UPDATE " << tableName << " SET "
            << "DHCP = \'" << param.dhcpValue << "\', "
            << "HTTP = \'" << param.httpValue << "\', "
            << "USER = \'" << param.userValue << "\' "
            << "WHERE ID = " << param.id;
            int rc = sqlite3_exec(database_, operation.str().c_str(), nullptr, nullptr, &lastErrorMsg_);
            if (rc != SQLITE_OK) {
                const std::string errorMsg = "Failed to update entry: " + param.name + ", " + lastErrorMsg_;
                sqlite3_free(lastErrorMsg_);
                lastErrorMsg_ = nullptr;
                throw std::domain_error(errorMsg);
            }
        }

        void createOrUpdateDefaultValue(const std::string_view tableName, const ParamEntity& param) {
            std::stringstream operation;
            operation << "INSERT INTO " << tableName <<
            " (ID, NAME, TYPE, DFLT) VALUES ("
            << param.id << ", "
            << "\'" << param.name << "\', "
            << "\'" << param.type << "\', "
            << "\'" << param.defaultValue << "\') "
            << "ON CONFLICT(ID) DO UPDATE SET DFLT=excluded.DFLT;";
            int rc = sqlite3_exec(database_, operation.str().c_str(), nullptr, nullptr, &lastErrorMsg_);
            if (rc != SQLITE_OK) {
                const std::string errorMsg = "Failed to update default value of entry: "
                + param.name + ", " + lastErrorMsg_;
                sqlite3_free(lastErrorMsg_);
                lastErrorMsg_ = nullptr;
                throw std::domain_error(errorMsg);
            }
        }

        void remove(const std::string_view tableName, const std::string_view name) {
            std::stringstream operation;
            operation << "DELETE FROM " << tableName << " WHERE NAME = \'" << name << "\'";
            int rc = sqlite3_exec(database_, operation.str().c_str(), nullptr, nullptr, &lastErrorMsg_);
            if (rc != SQLITE_OK) {
                const std::string errorMsg = "Failed to remove entry: " + std::string(name) + ", " + lastErrorMsg_;
                sqlite3_free(lastErrorMsg_);
                lastErrorMsg_ = nullptr;
                throw std::domain_error(errorMsg);
            }
        }

    private:
        static int readCallback(void* data, int argc, char** argv, char** azColName) {
            ParamEntity* const param = static_cast<ParamEntity*>(data);
            param->id = std::stoi(argv[0]);
            param->name = argv[1];
            param->type = argv[2];
            param->defaultValue = argv[3];
            param->dhcpValue = argv[4] ? argv[4] : "";
            param->httpValue = argv[5] ? argv[5] : "";
            param->userValue = argv[6] ? argv[6] : "";
            return 0;
        }

        static int readAllCallback(void* data, int argc, char** argv, char** azColName) {
            std::list<ParamEntity>* const allParams = static_cast<std::list<ParamEntity>*>(data);
            ParamEntity param;
            param.id = std::stoi(argv[0]);
            param.name = argv[1];
            param.type = argv[2];
            param.defaultValue = argv[3];
            param.dhcpValue = argv[4] ? argv[4] : "";
            param.httpValue = argv[5] ? argv[5] : "";
            param.userValue = argv[6] ? argv[6] : "";
            allParams->push_back(param);
            return 0;
        }

    private:
        sqlite3* database_;
        char* lastErrorMsg_;
    };
};

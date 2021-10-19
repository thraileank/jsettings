#include <catch2/catch.hpp>

#include "Table.hpp"
#include "../table/persistent/sqlite/SQLitePersistentTable.hpp"

using namespace JSettings;

TEST_CASE("SettingsTable set/get values if database just has been created", "[Settings]") {
    std::list<ParamVariant_t> dummyDefaultParameters {
        Param<std::string>(1, "MY_STRING_PARAM", ValueTypes::STRING, "My default value"),
        Param<int>(2, "MY_INTEGER_PARAM", ValueTypes::INTEGER, 3)
    };
    Table table(
        new SQLitePersistentTable("parameters_test.db", "PARAMETERS", dummyDefaultParameters)
    );
    table.init();
    
    SECTION("Can add a string value from a new source", "[Settings]") {
        table.setValue("MY_STRING_PARAM", SourceTypes::USER, std::string("My user value"));
        table.setValue("MY_STRING_PARAM", SourceTypes::HTTP, std::string("My http value"));
        std::string savedValue = table.getValue<std::string>("MY_STRING_PARAM");
        REQUIRE(savedValue == "My user value");
    }

    SECTION("Can add an integer value from a new source", "[Settings]") {
        table.setValue("MY_INTEGER_PARAM", SourceTypes::DHCP, 15);
        table.setValue("MY_INTEGER_PARAM", SourceTypes::HTTP, 10);
        REQUIRE(table.getValue<int>("MY_INTEGER_PARAM") == 10);
    }

    SECTION("Can update a string value from a new source", "[Settings]") {
        table.setValue("MY_STRING_PARAM", SourceTypes::HTTP, std::string("My old http value"));
        table.setValue("MY_STRING_PARAM", SourceTypes::HTTP, std::string("My new http value"));
        REQUIRE(table.getValue<std::string>("MY_STRING_PARAM", SourceTypes::HTTP) == "My new http value");
        // We should get a value from the highest priority source if no source specified
        // in a function call. We added a value with SourceTypes::USER source above,
        // so USER value still exists. The check below ensures that we get "My user value"
        // if call table.getValue without source specified
        REQUIRE(table.getValue<std::string>("MY_STRING_PARAM") == "My user value");
    }

    SECTION("Attempt to get non-existent value", "[Settings]") {
        REQUIRE_THROWS_AS(table.getValue<int>("UNKNOWN_NUMERIC_VALUE"), std::out_of_range); 
    }

    //TODO: Add tests for getting value by source
}

TEST_CASE("Persistent table database migration on changes in default table", "[Settings]") {
    std::list<ParamVariant_t> dummyDefaultParameters {
        Param<std::string>("MY_STRING_PARAM", ValueTypes::STRING, "My default value"),
        Param<int>("MY_INTEGER_PARAM", ValueTypes::INTEGER, 3),
        Param<std::string>("LAST_ACTIVE_USER", ValueTypes::STRING, "John C."),
        Param<int>("FAILED_LOGIN_ATTEMPTS", ValueTypes::INTEGER, 4),
        Param<std::string>("PROXY_SERVER", ValueTypes::STRING, "example.com")
    };

    {
        Table table(
            new SQLitePersistentTable("parameters_test_migrations.db", "PARAMETERS", dummyDefaultParameters)
        );
        table.init();
    }

    SECTION("New parameter was added to defaults map", "[Settings]") {
        dummyDefaultParameters = {
            Param<std::string>("MY_STRING_PARAM", ValueTypes::STRING, "My default value"),
            Param<int>("MY_INTEGER_PARAM", ValueTypes::INTEGER, 3),
            Param<std::string>("LAST_ACTIVE_USER", ValueTypes::STRING, "John C."),
            Param<int>("FAILED_LOGIN_ATTEMPTS", ValueTypes::INTEGER, 4),
            Param<std::string>("PROXY_SERVER", ValueTypes::STRING, "example.com"),
            Param<std::string>("MY_STRING_PARAM_2", ValueTypes::STRING, "My default value 2")
        };

        Table table(
            new SQLitePersistentTable("parameters_test_migrations.db", "PARAMETERS", dummyDefaultParameters)
        );
        table.init();
        REQUIRE(table.getValue<std::string>("MY_STRING_PARAM") == "My default value");
        REQUIRE(table.getValue<int>("MY_INTEGER_PARAM") == 3);
        REQUIRE(table.getValue<std::string>("LAST_ACTIVE_USER") == "John C.");
        REQUIRE(table.getValue<int>("FAILED_LOGIN_ATTEMPTS") == 4);
        REQUIRE(table.getValue<std::string>("PROXY_SERVER") == "example.com");
        REQUIRE(table.getValue<std::string>("MY_STRING_PARAM_2") == "My default value 2");
    }

    SECTION("A parameter removed from the table if it doesn't exist in a new revision", "[Settings]") {
        dummyDefaultParameters = {
            Param<std::string>("MY_STRING_PARAM", ValueTypes::STRING, "My default value"),
            Param<std::string>("LAST_ACTIVE_USER", ValueTypes::STRING, "John C."),
            Param<int>("FAILED_LOGIN_ATTEMPTS", ValueTypes::INTEGER, 4),
            Param<std::string>("PROXY_SERVER", ValueTypes::STRING, "example.com"),
            Param<std::string>("MY_STRING_PARAM_2", ValueTypes::STRING, "My default value 2")
        };

        Table table(
            new SQLitePersistentTable("parameters_test_migrations.db", "PARAMETERS", dummyDefaultParameters)
        );
        table.init();
        REQUIRE_THROWS(table.getValue<int>("MY_INTEGER_PARAM"));
    }

    SECTION("A parameter default value is updated", "[Settings]") {
        dummyDefaultParameters = {
            Param<std::string>("MY_STRING_PARAM", ValueTypes::STRING, "My new default value"),
            Param<std::string>("LAST_ACTIVE_USER", ValueTypes::STRING, "John C."),
            Param<int>("FAILED_LOGIN_ATTEMPTS", ValueTypes::INTEGER, 4),
            Param<std::string>("PROXY_SERVER", ValueTypes::STRING, "example.com"),
            Param<std::string>("MY_STRING_PARAM_2", ValueTypes::STRING, "My default value 2")
        };

        Table table(
            new SQLitePersistentTable("parameters_test_migrations.db", "PARAMETERS", dummyDefaultParameters)
        );
        table.init();
        REQUIRE(table.getValue<std::string>("MY_STRING_PARAM") == "My new default value");
    }

    SECTION("Add/remove more parameters", "[Settings]") {
        dummyDefaultParameters.remove_if([](auto& it){
            return std::visit(
                [](const auto& obj) -> bool {
                    return (obj.getName() == "MY_STRING_PARAM" || obj.getName() == "FAILED_LOGIN_ATTEMPTS");
                },
                it
            );
        });
        dummyDefaultParameters = {
            Param<std::string>("LAST_ACTIVE_USER", ValueTypes::STRING, "John B."),
            Param<std::string>("PROXY_SERVER", ValueTypes::STRING, "example.com"),
            Param<std::string>("MY_STRING_PARAM_2", ValueTypes::STRING, "My default value 2"),
            Param<int>("ACTIVE_CONNECTIONS", ValueTypes::INTEGER, 8)
        };

        std::function<void()> tableInit = [&dummyDefaultParameters]{
            Table table(
                new SQLitePersistentTable("parameters_test_migrations.db", "PARAMETERS", dummyDefaultParameters)
            );
            table.init();
        };
        REQUIRE_NOTHROW(tableInit());
    }
}
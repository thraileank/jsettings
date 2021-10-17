#include <catch2/catch.hpp>

#include "Table.hpp"
#include "../table/persistent/sqlite/SQLitePersistentTable.hpp"

using namespace JSettings;

TEST_CASE("SettingsTable set/get values if database just has been created", "[Settings]") {
    std::unordered_map<std::string, ParamVariant_t> dummyDefaultParameters {
        { "MY_STRING_PARAM", Param<std::string>(1, "MY_STRING_PARAM", ValueTypes::STRING, "My default value") },
        { "MY_INTEGER_PARAM", Param<int>(2, "MY_INTEGER_PARAM", ValueTypes::INTEGER, 3) }
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
    std::unordered_map<std::string, ParamVariant_t> dummyDefaultParameters {
        { "MY_STRING_PARAM", Param<std::string>(1, "MY_STRING_PARAM", ValueTypes::STRING, "My default value") },
        { "MY_INTEGER_PARAM", Param<int>(2, "MY_INTEGER_PARAM", ValueTypes::INTEGER, 3) }
    };

    {
        Table table(
            new SQLitePersistentTable("parameters_test_migrations.db", "PARAMETERS", dummyDefaultParameters)
        );
        table.init();
    }

    SECTION("New parameter was added to defaults map", "[Settings]") {
        dummyDefaultParameters["MY_STRING_PARAM_2"] = Param<std::string>(3, "MY_STRING_PARAM_2", ValueTypes::STRING, "My default value 2");
        Table table(
            new SQLitePersistentTable("parameters_test_migrations.db", "PARAMETERS", dummyDefaultParameters)
        );
        table.init();
        REQUIRE(table.getValue<std::string>("MY_STRING_PARAM") == "My default value");
        REQUIRE(table.getValue<int>("MY_INTEGER_PARAM") == 3);
        REQUIRE(table.getValue<std::string>("MY_STRING_PARAM_2") == "My default value 2");
    }

    SECTION("A parameter removed from the table if it doesn't exist in a new revision", "[Settings]") {
        dummyDefaultParameters.erase("MY_INTEGER_PARAMETER");
        Table table(
            new SQLitePersistentTable("parameters_test_migrations.db", "PARAMETERS", dummyDefaultParameters)
        );
        table.init();
        REQUIRE_THROWS(table.getValue<int>("MY_INTEGER_PARAMETER"));
    }

    SECTION("A parameter default value is updated", "[Settings]") {
        dummyDefaultParameters["MY_STRING_PARAM"] = Param<std::string>(1, "MY_STRING_PARAM", ValueTypes::STRING, "My new default value");
        Table table(
            new SQLitePersistentTable("parameters_test_migrations.db", "PARAMETERS", dummyDefaultParameters)
        );
        table.init();
        REQUIRE(table.getValue<std::string>("MY_STRING_PARAM") == "My new default value");
    }
}
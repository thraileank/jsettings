#include <catch2/catch.hpp>

#include "../table/Table.hpp"
#include "DummyTableDefaults.hpp"
#include "../table/persistent/sqlite/SQLitePersistentTable.hpp"

using namespace JSettings;

TEST_CASE("SettingsTable set/get values if database just has been created", "[Settings]") {
    Table table(
        new SQLitePersistentTable("parameters_test.db", "PARAMETERS", JSettings::DUMMY_DEFAULT_PARAMETERS)
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
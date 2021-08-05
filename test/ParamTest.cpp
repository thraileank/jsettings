#include <catch2/catch.hpp>

#include "../param/Param.hpp"

using namespace JSettings;

TEST_CASE("String parameter set/get values", "[Settings]") {
    Param<std::string> param { 1, "MY_STRING_PARAMETER", ValueTypes::STRING, "0" };
    param.setValue(SourceTypes::DHCP, "1");
    param.setValue(SourceTypes::USER, "3");

    REQUIRE(param.getValue(SourceTypes::DEFAULT) == "0");
    REQUIRE(param.getValue(SourceTypes::DHCP) == "1");
    REQUIRE(param.getValue(SourceTypes::USER) == "3");
    REQUIRE_THROWS_AS(param.getValue(SourceTypes::HTTP), std::out_of_range);

    SECTION("Can add a new source value", "[Settings]") {
        param.setValue(SourceTypes::HTTP, "2");
        REQUIRE(param.getValue(SourceTypes::HTTP) == "2");
    }

    SECTION("The most priority value returns if no source specified on get value", "[Settings]") {
        param.setValue(SourceTypes::HTTP, "2");
        REQUIRE(param.getValue() == "3");
    }

    SECTION("It's possible to update existing value", "[Settings]") {
        param.setValue(SourceTypes::USER, "4");
        REQUIRE(param.getValue(SourceTypes::USER) == "4");
    }

    SECTION("It's impossible to change default value", "[Settings]") {
        REQUIRE_THROWS_AS(param.setValue(SourceTypes::DEFAULT, "5"), std::invalid_argument);
    }

    SECTION("It's possible to delete an existing value", "[Settings]") {
        param.deleteValue(SourceTypes::USER);
        REQUIRE_THROWS_AS(param.getValue(SourceTypes::USER), std::out_of_range);
    }

    SECTION("It's impossible to delete a default value", "[Settings]") {
        REQUIRE_THROWS_AS(param.deleteValue(SourceTypes::DEFAULT), std::invalid_argument);
    }
}

TEST_CASE("Integer parameter set/get values", "[Settings]") {
    Param<int> param { 12345, "MY_INTEGER_PARAMETER", ValueTypes::INTEGER, -1 };
    param.setValue(SourceTypes::DHCP, 1);
    param.setValue(SourceTypes::USER, 3);

    REQUIRE(param.getValue(SourceTypes::DEFAULT) == -1);
    REQUIRE(param.getValue(SourceTypes::DHCP) == 1);
    REQUIRE(param.getValue(SourceTypes::USER) == 3);
    REQUIRE_THROWS_AS(param.getValue(SourceTypes::HTTP), std::out_of_range);

    SECTION("Can add a new source value", "[Settings]") {
        param.setValue(SourceTypes::HTTP, 2);
        REQUIRE(param.getValue(SourceTypes::HTTP) == 2);
    }

    SECTION("The most priority value returns if no source specified on get value", "[Settings]") {
        param.setValue(SourceTypes::HTTP, 2);
        REQUIRE(param.getValue() == 3);
    }

    SECTION("It's possible to update existing value", "[Settings]") {
        param.setValue(SourceTypes::USER, 4);
        REQUIRE(param.getValue(SourceTypes::USER) == 4);
    }

    SECTION("It's impossible to change default value", "[Settings]") {
        REQUIRE_THROWS_AS(param.setValue(SourceTypes::DEFAULT, 5), std::invalid_argument);
    }

    SECTION("It's possible to delete an existing value", "[Settings]") {
        param.deleteValue(SourceTypes::USER);
        REQUIRE_THROWS_AS(param.getValue(SourceTypes::USER), std::out_of_range);
    }

    SECTION("It's impossible to delete a default value", "[Settings]") {
        REQUIRE_THROWS_AS(param.deleteValue(SourceTypes::DEFAULT), std::invalid_argument);
    }
}
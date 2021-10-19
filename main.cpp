#include <iostream>

#include "Table.hpp"

int main() {
	
	std::list<JSettings::ParamVariant_t> parameters {
		JSettings::Param<int>("MY_INTEGER_PARAM", JSettings::ValueTypes::INTEGER, 5),
		JSettings::Param<std::string>("MY_STRING_PARAM", JSettings::ValueTypes::STRING, "TCP")
	};
	JSettings::Table table(
		new JSettings::SQLitePersistentTable("parameters.db", "PARAMETERS", parameters)
	);
	table.init();
	std::cout << table.getValue<int>("MY_INTEGER_PARAM") << std::endl;
	std::cout << table.getValue<std::string>("MY_STRING_PARAM") << std::endl;
	table.setValue("MY_STRING_PARAM", JSettings::SourceTypes::DHCP, std::string("TLS"));
	std::cout << table.getValue<std::string>("MY_STRING_PARAM") << std::endl;
	table.setValue("MY_STRING_PARAM", JSettings::SourceTypes::USER, std::string("UDP"));
	std::cout << table.getValue<std::string>("MY_STRING_PARAM") << std::endl;
	
	table.printAll();
	return 0;
}

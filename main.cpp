#include <iostream>

#include "./table/Table.hpp"
#include "./table/TableDefaults.hpp"

int main() {
	
	JSettings::Table table(
		new JSettings::SQLitePersistentTable("parameters.db", "PARAMETERS", JSettings::DEFAULT_PARAMETERS)
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

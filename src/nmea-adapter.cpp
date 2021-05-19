#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <stdio>

using json = nlohmann::json;

int NMEA_Adapter::readConfigFile(std::string path)
{
	std::ifstream ifs(path);
	json jf = json::parse(ifs);

	std::string str(R"({"json": "beta"})");
	json js = json::parse(str);
}


int main(int argc, char const *argv[])
{
	std::string conffile = "/etc/nmea_adapter.conf";

	// read in the json file
	if (argc > 1)
	{
		if (argv[2] == "-c")
			conffile = argv[3];
	}

	NMEA_Adapter* adapter = new NMEA_Adapter(conffile);

	int days = j.at("days");
	std::cout << days <<std::endl;

	return 0;
}

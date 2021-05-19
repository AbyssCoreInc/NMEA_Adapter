#include <iostream>
#include <fstream>
#include "nmea-adapter.hpp"

using json = nlohmann::json;

NMEA_Adapter::NMEA_Adapter(std::string path)
{
	
}

int NMEA_Adapter::readConfigFile(std::string path)
{
	std::ifstream ifs(path.c_str());
	this->conf = json::parse(ifs);
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


	return 0;
}

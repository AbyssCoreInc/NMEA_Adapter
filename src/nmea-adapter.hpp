#ifndef NMEA_ADAPTER_HPP
#define NMEA_ADAPTER_HPP

#define _GLIBCXX_USE_CXX11_ABI 0

#include <nlohmann/json.hpp>

class NMEA_Adapter
{
        nlohmann::json conf;

public:
	NMEA_Adapter(std::string path);
	int readConfigFile(std::string path);
};

#endif


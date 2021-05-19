#ifndef NMEA_ADAPTER_HPP
#define NMEA_ADAPTER_HPP

#define _GLIBCXX_USE_CXX11_ABI 0

#include <nlohmann/json.hpp>
#include <libusbp.hpp>
#include <iostream>
#include <iomanip>

class NMEA_Adapter
{
private:
        nlohmann::json conf;
	uint16_t vendor_id  = 0x0001;
	uint16_t product_id = 0x0002;
	int spfd = 0; // filedescriptor for serial device
	char read_buf[256];
	class mqtt_client *iot_client;
	std::string mqtt_host;
	std::string mqtt_port;
	std::string mqtt_clientid;

public:
	NMEA_Adapter(std::string path);
	int readConfigFile(std::string path);
	int openUSBSerialPort();
	std::string readSentence();
	unsigned int strToInt(const char *str, int base);
	int cleanUp();
};

#endif


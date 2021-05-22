#ifndef NMEA_ADAPTER_HPP
#define NMEA_ADAPTER_HPP

#include <nlohmann/json.hpp>
#include <libusbp.hpp>
#include <iostream>
#include <iomanip>
#include <pthread.h>
#include <sys/poll.h>
#include "nmea-interpreter.hpp"

class NMEA_Adapter
{
private:
        nlohmann::json conf;
	uint16_t vendor_id  = 0x0001;
	uint16_t product_id = 0x0002;
	int spfd = 0; // filedescriptor for serial device
	char read_buf[256];
	class mqtt_client *iot_client;
	NMEA_Interpreter* interpreter;
	std::string mqtt_host;
	std::string mqtt_port;
	std::string mqtt_clientid;
	struct pollfd fds[1];
public:
	pthread_t serial_thread;
	pthread_mutex_t serial_lock;
	pthread_mutex_t main_lock;
	pthread_cond_t serial_cond;

public:
	NMEA_Adapter(std::string path);
	int readConfigFile(std::string path);
	int openUSBSerialPort();
	std::string readSentence();
	void sendSentence(std::string sentence);
	void sendMQTTPacket(nlohmann::json data);
	unsigned int strToInt(const char *str, int base);
	int cleanUp();

	static void *readLoop(void *context);
};

#endif


#include <fstream>
#include "nmea-adapter.hpp"
#include "nmea-interpreter.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include "mqtt-client.hpp"
using json = nlohmann::json;

NMEA_Adapter::NMEA_Adapter(std::string path)
{
	class mqtt_client *iot_client;
	int rc;

	char client_id[] = CLIENT_ID;
	char host[] = BROKER_ADDRESS;
	int port = MQTT_PORT;

	std::cout<<" NMEA 0183 Fiware Adapther \n";
	this->readConfigFile(path);
	this->openUSBSerialPort();
	// initialize read buffer
	char read_buf[256];

	memset(&this->read_buf, '\0', sizeof(this->read_buf));

	iot_client = new mqtt_client(client_id, host, port);
}

unsigned int NMEA_Adapter::strToInt(const char *str, int base)
{
    const char digits[] = "0123456789abcdef";
    unsigned result = 0;

    while(*str)
    {
        result *= base;
        result += strchr(digits, *str++) - digits;
    }
    return result;
}

std::string NMEA_Adapter::readSentence()
{
	memset(&this->read_buf, '\0', sizeof(this->read_buf));
	int num_bytes = read(this->spfd, &this->read_buf, sizeof(this->read_buf));
	std::string ret(this->read_buf,sizeof(this->read_buf));
	//std::cout<<ret<<std::endl;
	return ret;
}

int NMEA_Adapter::openUSBSerialPort()
{
	struct termios options;

	//libusbp::device device = libusbp::find_device_with_vid_pid(0x067b,0x2303);
	libusbp::device device = libusbp::find_device_with_vid_pid(this->vendor_id, this->product_id);
	if (!device)
	{
		std::cerr << "Device not found: vid:"<<std::hex<<this->vendor_id<<" pid:"<<std::hex<<this->product_id<< std::endl;
		return 1;
	}

	libusbp::serial_port port(device);
	std::string port_name = port.get_name();
	std::cout << port_name << std::endl;

	this->spfd = open(port_name.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
	if (this->spfd == -1)
	{
		// Could not open the port. 
		perror("open_port: Unable to open /dev/ttyf1 - ");
		return 2;
	}
	fcntl(this->spfd, F_SETFL, 0);

	tcgetattr(this->spfd, &options);
	cfsetispeed(&options, B4800); // 4800 is the baudrate of NMEA0193
	options.c_cflag |= (CLOCAL | CREAD);
	tcsetattr(this->spfd, TCSANOW, &options);

	return 0;
}

int NMEA_Adapter::readConfigFile(std::string path)
{

	std::ifstream file(path);
	std::cout<<" Parse configuration file "<<path<<"\n";
	this->conf = json::parse(file);
	std::cout<<this->conf<<"\n";

	std::string s_vid = this->conf["vendor_id"];
	std::string s_pid = this->conf["product_id"];
	std::cout<<"vendor_id: "<<s_vid<<std::endl;
	std::cout<<"product_id: "<<s_pid<<std::endl;

	this->vendor_id = this->strToInt(s_vid.c_str(),16);
	this->product_id = this->strToInt(s_pid.c_str(),16);
}


int main(int argc, char const *argv[])
{
	std::string conffile = "/etc/nmea_adapter.conf";

	// read in the json file
	if (argc > 1)
	{
		std::cout<<"argv[1] "<<argv[1]<<" argv[1] "<<argv[2];
		if (strcmp(argv[1],"-c")==0)
			conffile = argv[2];
	}

	NMEA_Adapter* adapter = new NMEA_Adapter(conffile);
	NMEA_Interpreter* interpreter = new NMEA_Interpreter();
	nlohmann::json temp;
	while(1)
	{
		temp = interpreter->convertToJSON(adapter->readSentence());
		if (temp != NULL)
			std::cout<<temp<<std::endl;
	}
	adapter->cleanUp();
	return 0;
}


int NMEA_Adapter::cleanUp()
{
	close(this->spfd);
}

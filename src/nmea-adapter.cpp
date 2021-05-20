#include <fstream>
#include "nmea-adapter.hpp"
#include "nmea-interpreter.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <csignal>
#include "mqtt-client.hpp"
using json = nlohmann::json;

NMEA_Adapter::NMEA_Adapter(std::string path)
{
	this->mqtt_clientid = "1";
	std::cout<<" NMEA 0183 Fiware Adapther \n";
	this->readConfigFile(path);
	this->openUSBSerialPort();
	// initialize read buffer
	memset(&this->read_buf, '\0', sizeof(this->read_buf));

	//std::cout<<"init MQTT client"<<std::endl;
	this->iot_client = new mqtt_client(this->mqtt_clientid.c_str(), const_cast<char*>(this->mqtt_host.c_str()), this->strToInt(this->mqtt_port.c_str(),10));
	std::cout<<"NMEA Adapter ready"<<std::endl;
}

unsigned int NMEA_Adapter::strToInt(const char *str, int base)
{
    const char digits[] = "0123456789abcdef";
    unsigned int result = 0;

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

	this->mqtt_host = this->conf["mqtt_host"];
	this->mqtt_port = this->conf["mqtt_port"];
	this->vendor_id = this->strToInt(s_vid.c_str(),16);
	this->product_id = this->strToInt(s_pid.c_str(),16);

	return 0;
}

void NMEA_Adapter::sendMQTTPacket(nlohmann::json data)
{
	std::string topic = "/";
	topic.append(this->conf["api_key"]);
	topic.append("/");
	topic.append(data["sentence"]);
	topic.append("/attrs");
	std::cout<<"topic: "<<topic<<std::endl;
	data.erase("sentence");
	data.erase("source"); // WTF I shoudl do with this field is there a situation that there might be same sentence from two different devices?! 
	std::cout<<"data: "<<data<<std::endl;
	this->iot_client->publish_sensor_data(topic, data.dump());
}

/**
* This is global for cleanup function
*/
NMEA_Adapter* adapter;

void signalHandler( int signum ) {
	std::cout << "Interrupt signal (" << signum << ") received.\n";
	delete adapter;
	exit(signum);  
}

int main(int argc, char const *argv[])
{
	std::string conffile = "/etc/nmea_adapter.conf";
	signal(SIGINT, signalHandler);
	// read in the json file
	if (argc > 1)
	{
		std::cout<<"argv[1] "<<argv[1]<<" argv[1] "<<argv[2];
		if (strcmp(argv[1],"-c")==0)
			conffile = argv[2];
	}

	adapter = new NMEA_Adapter(conffile);
	NMEA_Interpreter* interpreter = new NMEA_Interpreter();

	nlohmann::json temp;
	while(1)
	{
		//std::cout<<"read next sentence"<<std::endl;
		temp = interpreter->convertToJSON(adapter->readSentence());
		if (temp != NULL)
		{
			std::cout<<"send data:"<<temp<<std::endl;
			adapter->sendMQTTPacket(temp);
		}
	}
	adapter->cleanUp();
	return 0;
}


int NMEA_Adapter::cleanUp()
{
	close(this->spfd);
	return 0;
}

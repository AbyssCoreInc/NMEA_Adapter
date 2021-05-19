#include <fstream>
#include "nmea-adapter.hpp"

using json = nlohmann::json;

NMEA_Adapter::NMEA_Adapter(std::string path)
{
	std::cout<<" NMEA 0183 Fiware Adapther \n";
	this->readConfigFile(path);
	this->openUSBSerialPort();
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



int NMEA_Adapter::openUSBSerialPort()
{
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


	return 0;
}

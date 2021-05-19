// Part for interpreting NMEA sentences to JSON structures
#include "nmea-interpreter.hpp"

NMEA_Interpreter::NMEA_Interpreter()
{
	;
}

nlohmann::json NMEA_Interpreter::convertToJSON(std::string data)
{
	// determine Sentence type
	nlohmann::json ret = NULL;
	std::string source = data.substr(1, 2);
	std::string type = data.substr(3, 3);
	std::string payload = data.substr(6, data.find("*")-6);
	if (payload.length() < data.length()-15)
	{
		//std::cout<<"size:"<<data.length()<<" dev:"<<source<<" type:"<<type<<" data:"<<payload<<std::endl;
		if (strcmp(type.c_str(),"DBT") == 0)
			ret = this->convertSentenceDBT(source,payload);
		else if (strcmp(type.c_str(),"VHW") == 0)
			ret = this->convertSentenceVHW(source,payload);
	}
	return ret;
}

nlohmann::json NMEA_Interpreter::convertSentenceDBT(std::string dev, std::string data)
{
	// IIDBT,63.7,f,19.8,M,,F*3D
	std::string delimiter = ",";
	nlohmann::json ret;
	int f_idx = data.find("f,");
	std::string temp = data.substr(f_idx+2, data.find(",M")-f_idx-2);
	//std::cout<<temp<<std::endl;
	ret["source"] = dev;
	ret["sentence"] = "DBT";
	ret["depth"] = temp;
	return ret;
}

nlohmann::json NMEA_Interpreter::convertSentenceVHW(std::string dev, std::string data)
{
	// $IIVHW,,,,,1.1,N,,*29
	std::string delimiter = ",";
        nlohmann::json ret;
	int t_idx = data.find(",T");  // True heading
	int m_idx = data.find(",M");  // Magnetic Heading
	int n_idx = data.find(",N");  // Speed in knots
	std::string temp;
	ret["source"] = dev;
	ret["sentence"] = "VHW";
	if (t_idx > 1)
	{
		temp = data.substr(0, t_idx-2); // this is first in the sentence so it can be started from 0 
		//std::cout<<temp<<std::endl;
		ret["heading_true"] = temp;
	}
	if (m_idx > 1)
	{
		temp = data.substr(m_idx-7, m_idx-2);
		//std::cout<<temp<<std::endl;
		ret["heading_magnetic"] = temp;
	}
	if (n_idx > 1)
	{
		temp = data.substr(n_idx-5, 5);
		int last = temp.find_last_of(delimiter);
		last++;
		temp = temp.substr(last, temp.length()-last);
		//std::cout<<temp<<std::endl;
		ret["speed_knots"] = temp;
	}
	return ret;
}

int NMEA_Interpreter::cleanUp()
{
	return 0;
}


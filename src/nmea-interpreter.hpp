#ifndef NMEA_INTERPRETER_HPP
#define NMEA_INTERPRETER_HPP

#include <nlohmann/json.hpp>
#include <iostream>
#include <iomanip>

class NMEA_Interpreter
{
private:
        nlohmann::json data;

public:
	NMEA_Interpreter();
	nlohmann::json convertToJSON(std::string data);
	/**
         * Functions for converting sentences to actual JSON structures*
         * Each sentence has its own function
         */
	nlohmann::json convertSentenceDBT(std::string dev, std::string data);
	nlohmann::json convertSentenceVHW(std::string dev, std::string data);
	int cleanUp();
};

#endif


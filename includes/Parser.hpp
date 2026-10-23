#pragma once

#include "Includes.hpp"

class Client;

void						entryParsing(int ac, char **av);
bool 						checkPort(const std::string &port);
bool 						isCorrectFormat(const std::string &msg);
std::vector<std::string> 	splitWithComma(std::string input);
std::string 				trim(std::string &buffer);
std::string 				checkPrefix(std::string &buffer);
std::string 				upperCase(std::string buffer);
std::string 				nickOrStar(const Client &client);

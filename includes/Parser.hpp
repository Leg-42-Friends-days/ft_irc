#pragma once

#include "Includes.hpp"
#include "Client.hpp"


std::string trim(std::string &buffer);
std::string removeDoubleDot(std::string &buffer);
std::string cutLine(std::string &buffer, int len);
std::string checkPrefix(std::string &buffer);
std::string upperCase(std::string buffer);
std::string nickOrStar(const Client &client);

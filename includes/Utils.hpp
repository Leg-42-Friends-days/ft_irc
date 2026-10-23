#pragma once

#include "Includes.hpp"

class Client;

void 		queueResponse(Client &client, const std::string &line);
std::string toLower(std::string str);
bool		isOnlyDigits(const std::string &str);

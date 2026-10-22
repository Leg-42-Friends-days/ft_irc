#pragma once

#include "Includes.hpp"

void 		queueResponse(Client &client, std::string line);
std::string toLower(std::string str);
bool		isOnlyDigits(const std::string &str);

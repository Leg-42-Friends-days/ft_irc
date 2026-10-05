#pragma once

#include "Includes.hpp"
#include "Message.hpp"

void sendResponse(const Client &client, std::string line);
std::string toLower(std::string str);
bool	isOnlyDigits(const std::string &str);
std::string convertToLine(const Message &message);
bool checkDot(const std::string &buffer);

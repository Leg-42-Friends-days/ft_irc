#pragma once

#include "Includes.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Message.hpp"

const std::string	BOT_CHANNEL		= "#eliza";
const std::string	BOT_NAME 		= "eliza";

std::string	ElizaAnswer(std::string const &message);

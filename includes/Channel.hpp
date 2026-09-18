#pragma once

#include "Includes.hpp"

class Channel
{
	private:
		std::string _channelName;
		std::map<int, Client*>	_members;
		std::map<int, Client*>	_operators;
		std::map<int, Client*>	_invited;
	public:

};
#include "Includes.hpp"
#include "Client.hpp"
#include "Replies.hpp"
#include "Utils.hpp"

void sendResponse(const Client &client, std::string line)
{
    line = line + "\r\n";
    send(client.getFdClient(), line.c_str(), line.size(), 0);
}

std::string toLower(std::string str)
{
	size_t i = 0;
	while(i < str.size())
	{
		str[i] = tolower(static_cast<unsigned char>(str[i]));
		i++;
	}
	return str;
}

bool	isOnlyDigits(const std::string &str)
{
	std::string::const_iterator	it = str.begin();
	while (it != str.end())
	{
		if (!std::isdigit(static_cast<unsigned char>(*it)))
			return (0);
		it++;
	}
	return (1);
}

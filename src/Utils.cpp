#include "../includes/Includes.hpp"
#include "../includes/Client.hpp"
#include "../includes/Replies.hpp"
#include "../includes/Utils.hpp"

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

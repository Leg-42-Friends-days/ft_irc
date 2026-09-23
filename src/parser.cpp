#include "../includes/Includes.hpp"
#include "../includes/Client.hpp"
#include "../includes/Replies.hpp"
#include "../includes/Server.hpp"

std::string trim(std::string &buffer)
{
	const std::string wspace = " \t\r\n";

	size_t first = buffer.find_first_not_of(wspace);

	if (first == std::string::npos)
		return "";

	size_t last = buffer.find_last_not_of(wspace);

	return (buffer.substr(first, (last - first + 1)));
}

std::string removeDoubleDot(std::string &buffer)
{
	int i = 0;
	if (buffer[0] == ':')
		i++;
	std::string line = buffer.substr(i);
	line = trim(line);
	return (line);
}

std::string cutLine(std::string &buffer, int len)
{
	int space = 0;
	for (size_t i = len; i < buffer.length(); i++)
	{
		if (!(std::isspace(buffer[i])))
			break;
		else
			space++;
	}
	
	return (buffer.substr(len + space));
}

std::string checkPrefix(std::string &buffer)
{
	if (buffer[0] == ':')
	{
		for (size_t i = 0; i < buffer.length(); i++)
		{
			if (std::isspace(buffer[i]))
			{
				std::string line = buffer.substr(i);
				line = trim(line);
				return (line);
			}
		}
	}
	return buffer;
}

std::string upperCase(std::string buffer)
{
	for (size_t i = 0; i < buffer.length(); i++)
		buffer[i] = toupper(static_cast<unsigned char>(buffer[i]));
	return (buffer);
}

std::string nickOrStar(const Client &client)
{
    if (client.getNickName().empty())
        return "*";
    return client.getNickName();
}

bool checkClientNickName(const std::string &msg, Server &serv)
{
    std::map<int, Client*> copy = serv.getClientRepo();

    for (std::map<int, Client*>::const_iterator it = copy.begin(); it != copy.end(); it++)
    {
        if (it->second->getNickName() == msg)
            return (true);
    }
    return (false);
}

bool checkClientUserName(const std::string &msg, Server &serv)
{
    std::map<int, Client*> copy = serv.getClientRepo();

    for (std::map<int, Client*>::const_iterator it = copy.begin(); it != copy.end(); it++)
    {
        if (it->second->getUserName() == msg)
            return (true);
    }
    return (false);
}
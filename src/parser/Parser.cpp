#include "Includes.hpp"
#include "Client.hpp"
#include "Replies.hpp"
#include "Server.hpp"

bool checkPort(const std::string &port)
{
	if(port.empty())
		return false;
	for (size_t i = 0; i < port.size(); i++)
	{
		if (!isdigit(static_cast<unsigned char>(port[i])))
				return false;
	}

	char *end;
	long portValue = std::strtol(port.c_str(), &end, 10);

	if(*end != '\0')
		return false;
	if (portValue > 65535)
			return false;
	return true;
}

void 	entryParsing(int &ac, char **av)
{
	if (ac != 3)
		throw std::runtime_error("execute : ./ircserv <port> <password>");
	if (!checkPort(av[1]))
		throw std::runtime_error("Error : invalid port!");
}

std::vector<std::string> splitWithComma(std::string input)
{
	std::vector<std::string>	output;
	std::string subStr;
	while (!input.empty())
 	{
		std::size_t found = input.find_first_of(",");
		if(found == std::string::npos)
		{
			output.push_back(input);
			break;
		}
		else
		{
			subStr = input.substr(0, found);
			if(!subStr.empty())
				output.push_back(subStr);
			input.erase(0, found + 1);
		}
  	}
	return (output);
}
std::string trim(std::string &buffer)
{
	const std::string wspace = " \t\r\n";

	size_t first = buffer.find_first_not_of(wspace);

	if (first == std::string::npos)
		return "";

	size_t last = buffer.find_last_not_of(wspace);

	return (buffer.substr(first, (last - first + 1)));
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

// Fonction qui passe une string en majuscule
std::string upperCase(std::string buffer)
{
	for (size_t i = 0; i < buffer.length(); i++)
		buffer[i] = toupper(static_cast<unsigned char>(buffer[i]));
	return (buffer);
}

// Fonction qui return le nickname, ou une etoile en cas d'absence de nickname
std::string nickOrStar(const Client &client)
{
    if (client.getNickName().empty())
        return "*";
    return client.getNickName();
}

bool checkFormat(const std::string &msg)
{
	for (size_t i = 0; i < msg.length(); i++)
	{
		if (static_cast<unsigned char>(std::ispunct(msg[i])))
			return (true);
	}
	return (false);
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

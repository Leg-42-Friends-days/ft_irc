#include "Includes.hpp"
#include "Client.hpp"
#include "Replies.hpp"
#include "Server.hpp"
#include "Utils.hpp"

// Fonction qui verifie la validite du port
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

// Fonction qui verifie nombre d'arguments et appel checkPort
void 	entryParsing(int &ac, char **av)
{
	if (ac != 3)
		throw std::runtime_error("execute : ./ircserv <port> <password>");
	if (!checkPort(av[1]))
		throw std::runtime_error("Error : invalid port!");
}

// Fonction qui decoupe une string sur les virgules
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

// Fonction qui trim les espaces et charettes
std::string trim(std::string &buffer)
{
	const std::string wspace = " \t\r\n";

	size_t first = buffer.find_first_not_of(wspace);

	if (first == std::string::npos)
		return "";

	size_t last = buffer.find_last_not_of(wspace);

	return (buffer.substr(first, (last - first + 1)));
}

// Fonction qui extrait prefixe du message si trouve
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

// Fonction qui check le format de Nickname
bool isCorrectFormat(const std::string &msg)
{
	if(msg.size() > 30)
		return false;

	const std::string specialCarac = "[]\\`_^{}|";

	if(!std::isalpha(static_cast<unsigned char>(msg[0])) && (specialCarac.find(msg[0]) == std::string::npos))
		return false;

	for (size_t i = 1; i < msg.length(); i++)
	{
		if(!std::isalpha(static_cast<unsigned char>(msg[i])) && (specialCarac.find(msg[i]) == std::string::npos)
		&& (static_cast<unsigned char>(msg[i]) != '-') && (!std::isdigit(static_cast<unsigned char>(msg[i]))))
			return false;
	}
	return true;
}

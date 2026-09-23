#include "../includes/Includes.hpp"
#include "../includes/Client.hpp"
#include "../includes/Replies.hpp"

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

void sendResponse(const Client &client, std::string line)
{
    line = line + "\r\n";
    send(client.getFdClient(), line.c_str(), line.size(), 0);
}


void assembleResponse(const Client &client, const char * code, const std::string &param, const std::string &text)
{
    std::ostringstream line;
    line << ':' << SERVER_NAME << " " << code << " " << nickOrStar(client);
    if(!param.empty())
        line <<  " " << param;
    if(!text.empty())
        line <<  " :" << text;       
    sendResponse(client, line.str());
}


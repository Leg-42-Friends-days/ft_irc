#include "../includes/Server.hpp"

// mise a jour pour gerer overflow
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

void	init_signals(void)
{
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, SIG_IGN);
}

void 	entryParsing(int &ac, char **av)
{
	//parsing de l'input du programme
	if (ac != 3)
		throw std::runtime_error("execute : ./ircserv <port> <password>");
	if (!checkPort(av[1]))
		throw std::runtime_error("Error : invalid port!");
}

void	pollLoop(Server &serv)
{
	while (true)
	{

		int	pollCount = poll(&serv.getpollFds()[0], serv.getpollFds().size(), -1);
		if (pollCount == -1)
		{
			std::cerr << "poll error" << std::endl;
			break;
		}
		size_t	i = 0;
		while (i < serv.getpollFds().size())
		{
			if (serv.getpollFds()[i].revents & POLLIN)
			{
				if (serv.getpollFds()[i].fd == serv.getServFd())
				{
					try
					{
						serv.addClient();
					}
					catch(const std::exception& e)
					{
						std::cerr << e.what() << '\n';
						continue ;
					}
				}
				else
				{
					try
					{
						serv.receiveMess(serv.getpollFds()[i]);
					}
					catch(const std::exception& e)
					{
						std::cerr << e.what() << '\n';
					}
				}
			}
			i++;
		}
	}
}


int main(int ac, char **av)
{
	try
	{
		entryParsing(ac, av);
		// enregistrer le password ?
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
	Server serv(av);
	//init_signals();
	// CTRL Z pour quitter
	try
	{
		serv.initServ();
		serv.initPollFds();
		serv.addChannel("general");
		serv.printChannels();
		pollLoop(serv);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

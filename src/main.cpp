#include "../includes/Server.hpp"

bool checkPort(std::string port)
{
		for (size_t i = 0; i < port.size(); i++)
		{
				if (!isdigit(port[i]))
						return (true);
		}

		int portValue = std::atoi(port.c_str());

		if (portValue < 0 || portValue > 65535)
				return (true);
		return (false);
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
	{
		std::cout << "execute : ./ircserv <port> <password>" << std::endl;
		throw std::exception();
	}

	if (checkPort(av[1]))
	{
		std::cout << "Error : invalid port!" << std::endl;
		throw std::exception();
	}
}

int main(int ac, char **av)
{
	try
	{
		entryParsing(ac, av);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return(1);
	}
	Server serv(av);
	//init_signals();
	// CTRL Z pour quitter
	try
	{
		serv.initServ();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return(1);
	}
	
	std::vector<struct pollfd> pollFds;
	struct pollfd	servPoll;
	servPoll.fd = serv.getServFd();
	servPoll.events = POLLIN;
	servPoll.revents = 0;
	pollFds.push_back(servPoll);
	
	while (true)
	{
		
		int	pollCount = poll(&pollFds[0], pollFds.size(), -1);
		if (pollCount == -1)
		{
			std::cerr << "poll error" << std::endl;
			break;
		}
		
		size_t	i = 0;
		while (i < pollFds.size())
		{
			if (pollFds[i].revents & POLLIN)
			{
				if (pollFds[i].fd == serv.getServFd())
				{
					try
					{
						serv.addClient(pollFds);
					}
					catch(const std::exception& e)
					{
						std::cerr << e.what() << '\n';
						continue ;
					}
				}
				else
				{
					//recevoir
					char buffer[4096];
				
					int message = recv(pollFds[i].fd, buffer, 4096, 0);
				
					if (message <= 0)
					{
						std::cout << "Client " << pollFds[i].fd << " Disconnected" << std::endl;
						close(pollFds[i].fd);
					}
					else
					{
						buffer[message] = '\0';
				
						std::cout << "Client " << pollFds[i].fd << " : " << buffer;
				
						/* size_t	j = 0;
						while (j < pollFds.size())
						{
							if (pollFds[j].fd == serv.getServFd())
								continue;
							send(pollFds[j].fd, buffer, message, 0);
							j++;
						} */
					}
				}
			}
			i++;
		}
	}

			


				

		// SINGLE SERVEUR TEST
		// sockaddr_in client;
		// socklen_t clientSize = sizeof(client);
		// char host[NI_MAXHOST];
		// char svc[NI_MAXSERV];

		// int clientSocket = accept(sockfd, (sockaddr*) &client, &clientSize);

		// if (clientSocket == -1)
		// {
		//   std::cerr << "Problem with client connecting!" << std::endl;
		//   return -4;
		// }

		// close(sockfd);

		// memset(host, 0, NI_MAXHOST);
		// memset(svc, 0, NI_MAXSERV);

		// int nameInfo = getnameinfo((sockaddr *) &client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);
		// if (nameInfo)
		// {
		//   std::cout << host << " connected on " << svc << std::endl;
		// }
		// else
		// {
		//   inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		//   std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;
		// }

		// char buff[4096];
		// while (true)
		// {
		//   memset(buff, 0, 4096);
		//   int bytesRecv = recv(clientSocket, buff, 4096, 0);
		//   if (bytesRecv == -1)
		//   {
		//     std::cerr << "There was a connection issue" << std::endl;
		//     break;
		//   }

		//   if (bytesRecv == 0)
		//   {
		//     std::cout << "The client disconnected" << std::endl;
		//     break;
		//   }

		//   std::cout << "Received: " << std::string(buff, 0, bytesRecv);

		//   send(clientSocket, buff, bytesRecv + 1, 0);
		// }

		// close(clientSocket);
		return EXIT_SUCCESS;
}
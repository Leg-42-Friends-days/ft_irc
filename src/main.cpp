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
	}
	
	Server serv(ac, av);
	init_signals();
	// CTRL Z pour quitter




		// socket
		int sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd == -1)
		{
			std::cerr << "Error : can't create a socket" << std::endl;
			return EXIT_FAILURE;
		}
		
		// bind
		sockaddr_in hint;
		hint.sin_family = AF_INET;
		// htons == host to networks short;
		hint.sin_port = htons(std::atoi(serv.getPortIP().c_str()));
		// internet command?? -- adress ip du pc == localhost;
		inet_pton(AF_INET, "127.0.0.1", &hint.sin_addr);

		if (bind(sockfd, (sockaddr *) &hint, sizeof(hint)) == -1)
		{
			std::cerr << "Can't bind to IP/port";
			return -2;
		}

		if (listen(sockfd, SOMAXCONN) == -1)
		{
			std::cerr << "Can't listen!" << std::endl;
			return -3;
		}

		fd_set master;
		fd_set copy;

		FD_ZERO(&master);
		FD_SET(sockfd, &master);

		int max_fd = sockfd;

		while (true)
		{
			copy = master;
			int result = select(max_fd + 1, &copy, NULL, NULL, NULL);
			if (result == -1)
			{
				std::cerr << "Error : Select didn't work";
				break;
			}
			
			for (int fd = 0; fd <= max_fd; ++fd)
			{
				if (!FD_ISSET(fd, &copy))
					continue;
				if (fd == sockfd)
				{
					sockaddr_in client_addr;
					socklen_t client_size = sizeof(client_addr);

					int client = accept(sockfd, (sockaddr *) &client_addr, &client_size);

					if (client == -1)
					{
						std::cerr << "Error : accept fonction didn't work";
						continue;
					}

					std::cout << "New client : " << client << " (" << inet_ntoa(client_addr.sin_addr) << ")" << std::endl;

					FD_SET(client, &master);

					if (client > max_fd)
						max_fd = client;
					const char* msg = "Welcome to the IRC server !\n";

					send(client, msg, std::strlen(msg), 0);
				}
				else
				{
					char buffer[4096];

					int bytes = recv(fd, buffer, 4096, 0);

					if (bytes <= 0)
					{
						std::cout << "Client " << fd << " Disconnected" << std::endl;
						close(fd);
						FD_CLR(fd, &master);
					}
					else
					{
						buffer[bytes] = '\0';

						std::cout << "Client " << fd << " : " << buffer;

						int other_fd;

						for (other_fd = 0; other_fd <= max_fd; ++other_fd)
						{
							if (other_fd == sockfd)
								continue;
							if (other_fd == fd)
								continue;
							if (!FD_ISSET(other_fd, &master))
								continue;
							send(other_fd, buffer, bytes, 0);
						}
					}
				}
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
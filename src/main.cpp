#include "../includes/.hpp"

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

int main(int ac, char **av)
{
    if (ac != 3)
    {
      std::cout << "execute : ./ircserv <port> <password>" << std::endl;
      return EXIT_FAILURE;
    }

    // CTRL Z pour quitter
    init_signals();

    std::string port = av[1];
    std::string password = av[2];

    if (checkPort(av[1]))
    {
      std::cout << "Error : invalid port!" << std::endl;
      return EXIT_FAILURE;
    }

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
    hint.sin_port = htons(std::atoi(port.c_str()));
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

    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];

    int clientSocket = accept(sockfd, (sockaddr*) &client, &clientSize);

    if (clientSocket == -1)
    {
      std::cerr << "Problem with client connecting!" << std::endl;
      return -4;
    }

    close(sockfd);

    memset(host, 0, NI_MAXHOST);
    memset(svc, 0, NI_MAXSERV);

    int nameInfo = getnameinfo((sockaddr *) &client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);
    if (nameInfo)
    {
      std::cout << host << " connected on " << svc << std::endl;
    }
    else
    {
      inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
      std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;
    }

    char buff[4096];
    while (true)
    {
      memset(buff, 0, 4096);
      int bytesRecv = recv(clientSocket, buff, 4096, 0);
      if (bytesRecv == -1)
      {
        std::cerr << "There was a connection issue" << std::endl;
        break;
      }

      if (bytesRecv == 0)
      {
        std::cout << "The client disconnected" << std::endl;
        break;
      }

      std::cout << "Received: " << std::string(buff, 0, bytesRecv);

      send(clientSocket, buff, bytesRecv + 1, 0);
    }

    close(clientSocket);
    return EXIT_SUCCESS;
}
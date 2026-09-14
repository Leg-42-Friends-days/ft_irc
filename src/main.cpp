#include "../includes/.hpp"

#define PORT 8080

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

int main(int ac, char **av)
{
    if (ac != 3)
    {
      std::cout << "execute : ./ircserv <port> <password>" << std::endl;
      return EXIT_FAILURE;
    }

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
    hint.sin_port = htons(54000);
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
      std::cout << host << " connected on " << "HERE" << std::endl;
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
      
      std::cout << "Received: " << std::string(buff, 0, bytesRecv) << std::endl;

      send(clientSocket, buff, bytesRecv + 1, 0);
    }

    close(clientSocket);
    return EXIT_SUCCESS;
}

// int main() {
//   int server_fd, new_socket;
//   struct sockaddr_in address;
//   int opt = 1;
//   int addrlen = sizeof(address);
//   char buffer[1024] = {0};

//   // Create socket
//   server_fd = socket(AF_INET, SOCK_STREAM, 0);
//   if (server_fd == 0) {
//     perror("socket failed");
//     exit(EXIT_FAILURE);
//   }

//   // Attach socket to the port
//   setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
//   address.sin_family = AF_INET;
//   address.sin_addr.s_addr = INADDR_ANY;
//   address.sin_port = htons(PORT);

//   // Bind
//   if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
//     perror("bind failed");
//     exit(EXIT_FAILURE);
//   }

//   // Listen
//   if (listen(server_fd, 3) < 0) {
//     perror("listen");
//     exit(EXIT_FAILURE);
//   }

//   // Accept a connection
//   new_socket =
//       accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
//   if (new_socket < 0) {
//     perror("accept");
//     exit(EXIT_FAILURE);
//   }

//   // Read data
//   read(new_socket, buffer, 1024);
//   std::cout << "Message from client: " << buffer << std::endl;

//   // Close socket
//   close(new_socket);
//   close(server_fd);
//   return 0;
// }
// LibC includes
# include <netinet/in.h>
# include <sys/socket.h>
# include <stdio.h>
# include <unistd.h>

// C++ includes
# include <iostream>
# include <cstring>

using namespace std;

# define PORT 6767
// curl -4 -v http://localhost:6767/

int main(void)
{
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket < 0)
    {
        perror("socket creation failed");
        return 1;
    }

    sockaddr_in serverAddress = {};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    const char* message = "Hello, server!";
    send(clientSocket, message, strlen(message), 0);

    close(clientSocket);

}
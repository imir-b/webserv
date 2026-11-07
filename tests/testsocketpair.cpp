#include <sys/socket.h>
#include <iostream>
#include <unistd.h>

int main(void)
{
    int sv[2];
    char buffer[100] = {0};

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0)
    {
        std::cerr << "socketpair failed" << std::endl;
        return 1;
    }

    std::cout << "Socket 1 : " << sv[0] << std::endl;
    std::cout << "Socket 2 : " << sv[1] << std::endl;

    // Envoyer un message depuis sv[0]
    write(sv[0], "Bonjour", 7);

    // Lire le message depuis sv[1]
    read(sv[1], buffer, sizeof(buffer) - 1);

    std::cout << "Message recu : " << buffer << std::endl;

    close(sv[0]);
    close(sv[1]);

    return 0;
}
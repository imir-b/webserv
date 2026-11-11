
/* HPP headers */
#include "Main.hpp"
#include "Server.hpp"

int main(int ac, char **av)
{
    if (ac != 2) {
        cout << "Usage: ./webserv [configuration file]" << endl;
        return 1;
    }

    Server server;

    if (server.parseConfig(av[1]) < 0) {
        cout << "Error: Can't parse configuration file" << endl;
        return 1;
    }

    signal(SIGPIPE, SIG_IGN);

    int serverSocket = server.setup();
    if (serverSocket < 0)
        return 1;

    cout << "Server is listening on port " << PORT << "..." << endl;

    int status = server.run();

    close(serverSocket);
    
    return status;
}

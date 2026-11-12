
/* HPP headers */
#include "ConfigParser.hpp"
#include "Main.hpp"
#include "Server.hpp"

int main(int ac, char **av)
{
    if (ac != 2) {
        cerr << "Usage: ./webserv [configuration file]" << endl;
        return 1;
    }

    ConfigParser parser;
    Server server;
    
    if (parser.parseConfig(av[1], server) < 0) {
        cerr << "Error: Can't parse configuration file" << endl;
        return 1;
    }

    // cout << "Config after parsing :" << endl;
    // cout << server.getConfig() << endl << endl;

    signal(SIGPIPE, SIG_IGN);

    int serverSocket = server.setup();
    if (serverSocket < 0)
        return 1;

    cout << "Server is listening on port " << PORT << "..." << endl;

    int status = server.run();

    close(serverSocket);
    return status;
}

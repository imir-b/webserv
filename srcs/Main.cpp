
/* HPP headers */
#include "../headers/ConfigParser.hpp"
#include "../headers/Main.hpp"
#include "../headers/Server.hpp"

int main(int ac, char **av) {
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

    const vector<Config> &configs = server.getConfigs();

    for (size_t i = 0; i < configs.size(); i++) {
        cout << "Virtual server " << i + 1 << ":" << endl;
        cout << configs[i] << endl;
    }

    signal(SIGPIPE, SIG_IGN);

    if (server.setupSockets() < 0)
        return 1;

    cout << "All servers are listening..." << endl;

    return server.run();
}

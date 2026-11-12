#pragma once

/* LibC includes */
#include <poll.h>

/* C++ includes */
#include <vector>
#include <map>
#include <sstream>
#include <cstdlib>
#include <climits>
#include <cctype>
#include <fstream>
#include <algorithm>

/* HPP headers */
#include "Client.hpp"
#include "Config.hpp"
#include "RequestParser.hpp"
#include "RequestHandler.hpp"

using namespace std;

struct ListeningSocket {
    int fd;
    string host;
    int port;
};

class Server {
    private:
        /* Attributes */
        vector<ListeningSocket> serverSockets;
        map<int, ListeningSocket> clientListeners;
        vector<struct pollfd> fds;
        map<int, Client> clients;

        /* Server configuration, define in configuration file */
        vector<Config> configs;

        /* Request Parser class to manage request parsing */
        RequestParser requestParser;

        /* Request Handler class to handle request methods */
        RequestHandler requestHandler;

    public:
        /* Constructor */
        Server();
        ~Server();

        /* Configs */
        void addConfig(const Config &config);
        const vector<Config> &getConfigs() const;
        const Config &selectConfig(const Request &request, int clientFd) const;

        /* Methods */
        int setup();
        int setupSocket(const string &host, int port);
        int setupSockets();
        int run();
        int processEvents();
        bool processClientEvent(size_t index);
        bool processClientRead(size_t index, Client &client, bool &peerClosed);
        bool processClientWrite(size_t index, Client &client);
        bool processClientRequest(size_t index, Client &client, bool peerClosed);
        int processNewConnections(int listeningFd);
        int acceptClient(int listeningFd);
        bool isListeningSocket(int fd) const;
        int handleClient(Client &client);
        int processRequest(Client &client);
        int sendResponse(Client &client);
        int sendErrorBadRequest(Client &client);
        void removeClient(size_t index);
};

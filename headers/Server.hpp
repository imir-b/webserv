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

class Server {
    private:
        /* Attributes */
        int serverSocket;
        vector<struct pollfd> fds;
        map<int, Client> clients;

        /* Server configuration, define in configuration file */
        Config config;

        /* Request Parser class to manage request parsing */
        RequestParser requestParser;

        /* Request Handler class to handle request methods */
        RequestHandler requestHandler;

    public:
        /* Constructor */
        Server();
        ~Server();

        /* Setters */
        void setConfig(Config &config);

        /* Methods */
        int setup();
        int run();
        int processEvents();
        bool processClientEvent(size_t index);
        bool processClientRead(size_t index, Client &client, bool &peerClosed);
        bool processClientWrite(size_t index, Client &client);
        bool processClientRequest(size_t index, Client &client, bool peerClosed);
        int processNewConnections();
        int acceptClient();
        int handleClient(Client &client);
        int processRequest(Client &client);
        int sendResponse(Client &client);
        int sendErrorBadRequest(Client &client);
        void removeClient(size_t index);
};

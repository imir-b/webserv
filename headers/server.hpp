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
#include <algorithm>

/* HPP headers */
#include "client.hpp"
#include "requestParser.hpp"

using namespace std;

class Server {
    private:
        /* Attributes */
        int serverSocket;
        vector<struct pollfd> fds;
        map<int, Client> clients;

        /* Request Parser class to manage request parsing */
        RequestParser requestParser;

    public:
        /* Constructor */
        Server();
        ~Server();

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
        int handleGet(Client &client);
        int handlePost(Client &client);
        int handleDelete(Client &client);
        int handleMethodNotAllowed(Client &client);
        int sendResponse(Client &client);
        int sendErrorBadRequest(Client &client);
        void removeClient(size_t index);
};

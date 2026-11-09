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

#include "client.hpp"
#include "requestParser.hpp"

using namespace std;

class Server {
    private:
        int serverSocket;
        vector<struct pollfd> fds;
        map<int, Client> clients;

        RequestParser requestParser;

    public:
        Server();
        ~Server();

        int setup();
        int run();
        int processEvents();
        bool processClientEvent(size_t index);
        int processNewConnections();
        int acceptClient();
        int handleClient(Client &client);
        int processRequest(Client &client);
        int sendResponse(Client &client);
        void sendErrorBadRequest();
        void removeClient(size_t index);
};

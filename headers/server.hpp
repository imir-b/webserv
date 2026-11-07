#pragma once

#include <poll.h>

#include <vector>
#include <map>

#include "client.hpp"

using namespace std;

class Server {
    private:
        int serverSocket;
        vector<struct pollfd> fds;
        vector<struct pollfd> pendingClients;
        map<int, Client> clients;
    public:
        Server();
        ~Server();

        int setup();
        int run();
        int processEvents();
        int acceptClient();
        void removeClient(size_t index);   
};

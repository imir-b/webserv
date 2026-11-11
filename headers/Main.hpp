# pragma once

/* LibC includes */
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>

/* C++ includes */
#include <cerrno>
#include <csignal>
#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <utility>

/* HPP headers */
#include "Client.hpp"

using namespace std;

/* Constants */
#define PORT 6767       // Port used by the server to listen for connections
#define BACKLOG 5       // Maximum number of pending connections in the queue
#define BUFSIZE 1024    // Maximum number of bytes read per recv() call
#define MAX_READS 10    // Maximum number of recv() calls per client per event

/* Client states */
#define CLIENT_CONNECTED 0
#define CLIENT_DISCONNECTED 1
#define CLIENT_ERROR -1


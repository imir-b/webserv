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
#include "client.hpp"

using namespace std;

/* Constants */
#define PORT 6767
#define BACKLOG 5
#define BUFSIZE 1024

/* Client states */
#define CLIENT_CONNECTED 0
#define CLIENT_DISCONNECTED 1
#define CLIENT_ERROR -1

/* Client functions */
int handle_client(Client &client);

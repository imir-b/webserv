
/* LibC includes */
#include <arpa/inet.h>

/* HPP headers */
#include "Main.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "Response.hpp"

/* Constructor */

Server::Server(void) {}

Server::~Server(void) {}

/**
 * Utils
 */
/* Set the socket to non-blocking */
int set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);

    if (flags < 0) {
        perror("fcntl F_GETFL");
        return -1;
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("fcntl F_SETFL");
        return -1;
    }

    return 0;
}

/* Add a socket to pollfd */
void add_pollfd(vector<struct pollfd> &fds, int fd)
{
    struct pollfd pfd;

    pfd.fd = fd;
    pfd.events = POLLIN;
    pfd.revents = 0;

    fds.push_back(pfd);
}

/* Receive data from a client and store it in the client read buffer */
int Server::handleClient(Client &client)
{
    char buffer[BUFSIZE];
    int readCount = 0;

    while (readCount < MAX_READS)
    {
        ssize_t bytes = recv(client.getFd(), buffer, sizeof(buffer), 0);

        if (bytes > 0)
        {
            if (!client.appendReadBuffer(buffer, static_cast<size_t>(bytes)))
                return CLIENT_ERROR; // A remplacer plus tard par une gestion HTTP 413 <<<---
            cout << endl << "Read buffer client (fd " << client.getFd() << "): " << endl;
            cout.write(client.getReadBuffer().data(), client.getReadBuffer().size());
            cout << endl;

            ++readCount;
            continue;
        }

        /* All data has been received, client disconnected */
        if (bytes == 0)
        {
            cout << "Client disconnected (fd "
                 << client.getFd() << ")" << endl;

            return CLIENT_DISCONNECTED;
        }

        /* Call interrupted by a signal */
        if (errno == EINTR)
            continue;

        /* All disponible data has been receive */
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return CLIENT_CONNECTED;

        perror("recv");
        return CLIENT_ERROR;
    }

    return CLIENT_CONNECTED;
}

/* Close and remove a client */
void Server::removeClient(size_t index)
{
    int fd = fds[index].fd;
    close(fd);
    clients.erase(fd);
    fds.erase(fds.begin() + index);
}

/* Accept a new connection */
int Server::acceptClient(void)
{
    int clientSocket = accept(serverSocket, NULL, NULL);
    if (clientSocket < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
            return 0;
        perror("accept");
        return -1;
    }

    if (set_nonblocking(clientSocket) < 0) {
        close(clientSocket);
        return -1;
    }

    clients.insert(make_pair(clientSocket, Client(clientSocket)));

    add_pollfd(fds, clientSocket);

    cout << "Accepted a new connection (fd "
         << clientSocket << ")." << endl;


    return 0;
}

/* Accept new connections after processing existing clients */
int Server::processNewConnections(void)
{
    if (acceptClient() < 0)
        return -1;

    return 0;
}

/* Prepare a 400 Bad Request response */
int Server::sendErrorBadRequest(Client &client)
{
    Response response;

    response.setStatusCode(HTTP_BAD_REQUEST);
    response.setHeader("Content-Type", "text/plain");
    response.setHeader("Connection", "close");
    response.setBody("400 Bad Request\n");

    if (!client.appendWriteBuffer(response.serialize()))
        return -1;
    client.setCloseAfterResponse(true);

    return 0;
}
/* Send the response in writeBuffer to the client socket */
int Server::sendResponse(Client &client)
{
    const int MAX_SENDS = 10;
    int sendCount = 0;

    const string& buffer = client.getWriteBuffer();

    while (client.getBytesSent() < buffer.size() && sendCount < MAX_SENDS) {
        size_t remaining = buffer.size() - client.getBytesSent();

        ssize_t bytes = send(client.getFd(), buffer.data() + client.getBytesSent(), remaining, 0);

        /* Data successfully sent */
        if (bytes > 0) {
            client.addBytesSent(bytes);
            ++sendCount;
            continue;
        }

        /* No progress */
        if (bytes == 0)
            return -1;

        /* Call interrupted by a signal */
        if (errno == EINTR)
            continue;

        /* Socket temporarily unavailable */
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return 0;

        perror("send");
        return -1;
    }

    /* Entire response has been sent */
    if (client.getBytesSent() == buffer.size())
        client.clearWriteBuffer();

    return 0;
}

/* Parse and process the client's HTTP request */
bool Server::processClientRequest(size_t index, Client &client, bool peerClosed)
{
    while (!client.getReadBuffer().empty()) {
        size_t consumed = 0;

        int ret = requestParser.parse(client.getReadBuffer(), client.getRequest(), consumed);

        if (ret == INCOMPLETE_REQUEST) {
            /* Client disconnected before completing the request */
            if (peerClosed) {
                if (sendErrorBadRequest(client) < 0) {
                    removeClient(index);
                    return true;
                }

                fds[index].events |= POLLOUT;
            }

            break;
        }

        if (ret == INVALID_REQUEST || consumed == 0) {
            /* Prepare a 400 Bad Request response */
            if (sendErrorBadRequest(client) < 0) {
                removeClient(index);
                return true;
            }

            fds[index].events |= POLLOUT;
            break;
        }

        cout << client.getRequest() << endl;

        Response response = requestHandler.process(client.getRequest());
        
        client.consumeReadBuffer(consumed);
        if (!client.appendWriteBuffer(response.serialize())) {
            return false; // supprimer le client proprement <<<---
        }
        client.setCloseAfterResponse(true);

        /* Enable POLLOUT when response is ready */
        if (!client.getWriteBuffer().empty())
            fds[index].events |= POLLOUT;

        /* One request per connection for now */
        break;
    }

    return false;
}

/* Send response data to the client */
bool Server::processClientWrite(size_t index, Client &client)
{
    if (sendResponse(client) < 0) {
        removeClient(index);
        return true;
    }

    /* All response data has been sent */
    if (client.getWriteBuffer().empty()) {
        fds[index].events &= ~POLLOUT;

        if (client.shouldCloseAfterResponse()) {
            removeClient(index);
            return true;
        }
    }

    return false;
}

/* Receive data from an existing client */
bool Server::processClientRead(size_t index, Client &client, bool &peerClosed)
{
    cout << "Entering processClientRead - fd " << client.getFd() << endl; // debug <<<---
    int ret = handleClient(client);

    if (ret == CLIENT_ERROR) {
        removeClient(index);
        return true;
    }

    /* Client has closed its sending side */
    if (ret == CLIENT_DISCONNECTED || (fds[index].revents & POLLHUP))
        peerClosed = true;

    return processClientRequest(index, client, peerClosed);
}

/* Process existing client socket */
bool Server::processClientEvent(size_t index)
{
    int fd = fds[index].fd;
    short events = fds[index].revents;

    map<int, Client>::iterator it = clients.find(fd);

    if (it == clients.end()) {
        removeClient(index);
        return true;
    }

    Client *client = &(it->second);
    bool peerClosed = false;

    /* Client socket: error */
    if (events & (POLLERR | POLLNVAL)) {
        removeClient(index);
        return true;
    }

    /* Client socket: data available or disconnected */
    if (events & (POLLIN | POLLHUP)) {
        cout << "request catch" << endl; // debug <<<---
        if (processClientRead(index, *client, peerClosed))
            return true;
    }

    /* Client socket: data can be sent */
    if (events & POLLOUT) {
        cout << "response send" << endl; // debug <<<---
        if (processClientWrite(index, *client))
            return true;
    }

    /* Client socket: disconnected */
    if (peerClosed && client->getWriteBuffer().empty()) {
        removeClient(index);
        return true;
    }

    return false;
}

/* Process events reported by poll */
int Server::processEvents(void)
{
    bool newConnection = false;

    for (size_t i = 0; i < fds.size(); ) {
        short events = fds[i].revents;

        /* Server socket */
        if (fds[i].fd == serverSocket) {
            if (events & (POLLERR | POLLHUP | POLLNVAL)) {
                cerr << "Listening socket error" << endl;
                return -1;
            }

            if (events & POLLIN)
                newConnection = true;

            ++i;
            continue;
        }

        /* Process existing client */
        if (processClientEvent(i))
            continue;

        ++i;
    }

    /* Accept new connections after processing existing clients */
    if (newConnection)
        return processNewConnections();

    return 0;
}

/* Main event loop */
int Server::run(void)
{
    int status = 0;
    
    add_pollfd(fds, serverSocket);

    while (true) {
        int ret = poll(&fds[0], fds.size(), -1);

        if (ret < 0) {
            if (errno == EINTR)
                continue;

            perror("poll");
            status = 1;
            break;
        }

        if (processEvents() < 0) {
            status = 1;
            break;
        }
    }

    /* Close remaining client sockets */
    for (size_t i = 1; i < fds.size(); ++i)
        close(fds[i].fd);

    return status;
}

/* Create, configure and start listening */
int Server::setup(void)
{
    /* create the server socket */
    Server::serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("socket");
        return -1;
    }

    /* configure option REUSEADDR to enable resusing server socket */
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(serverSocket);
        return -1;
    }

    /* configure sockaddr structure */
    sockaddr_in address = {};
    address.sin_family = AF_INET;
    address.sin_port = htons(config.getListen());

    if (inet_pton(AF_INET, config.getHost().c_str(), &address.sin_addr) != 1) {
        cerr << "Invalid IP address: " << config.getHost() << endl;
        close(serverSocket);
        return -1;
    }

    /* bind server socket with sockaddr structure */
    if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind");
        close(serverSocket);
        return -1;
    }

    /* define backlog with listen */
    if (listen(serverSocket, BACKLOG) < 0) {
        perror("listen");
        close(serverSocket);
        return -1;
    }

    /* set the server socket non-blocking */
    if (set_nonblocking(serverSocket) < 0) {
        close(serverSocket);
        return -1;
    }

    return serverSocket;
}

const Config &Server::getConfig(void) const {
        return config;
}

Config &Server::getConfig(void) {
    return config;
}

void Server::setConfig(Config &_config) {
    config = _config;
}

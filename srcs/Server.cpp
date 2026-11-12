
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
    clientListeners.erase(fd);
    clients.erase(fd);
    fds.erase(fds.begin() + index);
}

bool Server::isListeningSocket(int fd) const
{
    for (size_t i = 0; i < serverSockets.size(); i++) {
        if (serverSockets[i].fd == fd)
            return true;
    }

    return false;
}

/* Accept a new connection */
int Server::acceptClient(int listeningFd) {
    int clientSocket = accept(listeningFd, NULL, NULL);

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

    bool found = false;

    for (size_t i = 0; i < serverSockets.size(); i++) {
        if (serverSockets[i].fd == listeningFd) {
            clientListeners[clientSocket] = serverSockets[i];
            found = true;
            break;
        }
    }

    if (!found) {
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
int Server::processNewConnections(int listeningFd) {
    while (true) {
        int ret = acceptClient(listeningFd);

        if (ret < 0)
            return -1;

        break;
    }

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

        const Config &config = selectConfig(client.getRequest(), client.getFd());
        Response response = requestHandler.process(client.getRequest(), config);
        
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
    vector<int> listeningSockets;

    for (size_t i = 0; i < fds.size(); ) {
        int fd = fds[i].fd;
        short events = fds[i].revents;

        if (isListeningSocket(fd)) {
            if (events & (POLLERR | POLLHUP | POLLNVAL)) {
                cerr << "Listening socket error" << endl;
                return -1;
            }

            if (events & POLLIN)
                listeningSockets.push_back(fd);

            ++i;
            continue;
        }

        if (processClientEvent(i))
            continue;

        ++i;
    }

    for (size_t i = 0; i < listeningSockets.size(); i++) {
        if (processNewConnections(listeningSockets[i]) < 0)
            return -1;
    }

    return 0;
}

/* Main event loop */
int Server::run(void)
{
    int status = 0;
    
    for (size_t i = 0; i < serverSockets.size(); i++)
        add_pollfd(fds, serverSockets[i].fd);

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

int Server::setupSocket(const string &host, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        perror("socket");
        return -1;
    }

    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(sock);
        return -1;
    }

    sockaddr_in address = {};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &address.sin_addr) != 1) {
        cerr << "Invalid IP address: " << host << endl;
        close(sock);
        return -1;
    }

    if (bind(sock, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind");
        close(sock);
        return -1;
    }

    if (listen(sock, BACKLOG) < 0) {
        perror("listen");
        close(sock);
        return -1;
    }

    if (set_nonblocking(sock) < 0) {
        close(sock);
        return -1;
    }

    return sock;
}

int Server::setupSockets(void) {
    for (size_t i = 0; i < configs.size(); i++) {
        bool alreadyExists = false;

        for (size_t j = 0; j < serverSockets.size(); j++) {
            if (serverSockets[j].host == configs[i].getHost()
                && serverSockets[j].port == configs[i].getListen()) {
                alreadyExists = true;
                break;
            }
        }

        if (alreadyExists)
            continue;

        int sock = setupSocket(configs[i].getHost(), configs[i].getListen());

        if (sock < 0)
            return -1;

        ListeningSocket listening;

        listening.fd = sock;
        listening.host = configs[i].getHost();
        listening.port = configs[i].getListen();

        serverSockets.push_back(listening);
    }

    return 0;
}

void Server::addConfig(const Config &config) {
    configs.push_back(config);
}

const vector<Config> &Server::getConfigs() const {
    return configs;
}

const Config &Server::selectConfig(const Request &request, int clientFd) const {
    map<int, ListeningSocket>::const_iterator it;
    it = clientListeners.find(clientFd);

    if (configs.empty())
        throw runtime_error("No server configuration available");

    if (it == clientListeners.end())
        return configs[0];

    const ListeningSocket &listening = it->second;
    string host = request.getHeader("Host");

    /* Remove port from HTTP Host */
    size_t colon = host.find(':');

    if (colon != string::npos)
        host.erase(colon);

    /* Search matching virtual host */
    for (size_t i = 0; i < configs.size(); i++) {
        if (configs[i].getHost() == listening.host
            && configs[i].getListen() == listening.port
            && configs[i].getServerName() == host)
            return configs[i];
    }

    /* Return default virtual host */
    for (size_t i = 0; i < configs.size(); i++) {
        if (configs[i].getHost() == listening.host
            && configs[i].getListen() == listening.port)
            return configs[i];
    }

    return configs[0];
}

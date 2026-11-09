
/* Header files */
#include "main.hpp"
#include "client.hpp"
#include "server.hpp"

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
        ssize_t bytes = recv(client.getFd(), buffer,
                         sizeof(buffer), 0);

        if (bytes > 0)
        {
            client.appendReadBuffer(buffer, bytes);
            cout << "Read buffer client (fd " << client.getFd() << "): ";
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
Server::Server(void) {}

Server::~Server(void) {}

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

void Server::sendErrorBadRequest(void) {

}

/* Send the response in writeBuffer to the client socket */
int Server::sendResponse(Client &client)
{
    (void) client;
    return 0;
}

/* Process the client's request and prepare the HTTP response */
int Server::processRequest(Client &client)
{
    (void) client;
    return 0;
}

/* Process existing client socket */
bool Server::processClientEvent(size_t index)
{
    int fd = fds[index].fd;
    map<int, Client>::iterator it = clients.find(fd);
    short events = fds[index].revents;

    if (it == clients.end())
        return false;

    Client *client = &(it->second);

    /* Client socket: error */
    if (events & (POLLERR | POLLNVAL))
    {
        removeClient(index);
        return true;
    }

    /* Client socket: data available */
    if (events & POLLIN)
    {
        int ret = handleClient(*client);

        if (ret == CLIENT_DISCONNECTED || ret == CLIENT_ERROR) // Attention à la déconnexion avant le parsing <<<---
        {
            removeClient(index);
            return true;
        }

        /* Parse client's request */
        while (!client->getReadBuffer().empty())
        {
            size_t consumed = 0;

            int ret = requestParser.parse(client->getReadBuffer(), client->getRequest(), consumed);

            if (ret == INCOMPLETE_REQUEST)
                break;

            if (ret == INVALID_REQUEST)
            {
                // préparer une réponse HTTP 400 Bad Request; définir si la connexion doit être fermée après cette réponse; activer POLLOUT pour envoyer la réponse <<<---
                break;
            }

            client->consumeReadBuffer(consumed);

            if (processRequest(*client) < 0)
                return false;

            // Attention : si une réponse est déjà en attente d'envoi, il faut gérer correctement le pipeline HTTP. <<<---
        }
    }

    /* Client socket: data can be sent */
    if (events & POLLOUT)
    {
        if (sendResponse(*client) < 0)
        {
            removeClient(index);
            return true;
        }

        if (client->getWriteBuffer().empty())
        {
            fds[index].events &= ~POLLOUT;

            if (client->shouldCloseAfterResponse())
            {
                removeClient(index);
                return true;
            }
        }
    }

    /* Client socket: disconnected */
    if (events & POLLHUP)
    {
        removeClient(index);
        return true;
    }

    return false;
}

/* Process events reported by poll */
int Server::processEvents(void)
{
    bool newConnection = false;

    for (size_t i = 0; i < fds.size(); )
    {
        short events = fds[i].revents;

        /* Server socket */
        if (fds[i].fd == serverSocket)
        {
            if (events & (POLLERR | POLLHUP | POLLNVAL))
            {
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
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR,
                   &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(serverSocket);
        return -1;
    }

    /* configure sockaddr structure */
    sockaddr_in address = {};
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    /* bind server socket with sockadrr structure */
    if (bind(serverSocket, (struct sockaddr *)&address,
             sizeof(address)) < 0) {
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

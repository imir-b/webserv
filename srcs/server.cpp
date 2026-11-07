
/* Header files */
#include "main.hpp"
#include "client.hpp"


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

/* Create, configure and start listening */
int setup_server_socket(void)
{
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("socket");
        return -1;
    }

    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR,
                   &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(serverSocket);
        return -1;
    }

    sockaddr_in address = {};
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(serverSocket, (struct sockaddr *)&address,
             sizeof(address)) < 0) {
        perror("bind");
        close(serverSocket);
        return -1;
    }

    if (listen(serverSocket, BACKLOG) < 0) {
        perror("listen");
        close(serverSocket);
        return -1;
    }

    if (set_nonblocking(serverSocket) < 0) {
        close(serverSocket);
        return -1;
    }

    return serverSocket;
}

/* Add a socket to poll */
void add_pollfd(vector<struct pollfd> &fds, int fd)
{
    struct pollfd pfd;

    pfd.fd = fd;
    pfd.events = POLLIN;
    pfd.revents = 0;

    fds.push_back(pfd);
}

/* Accept a new connection */
int accept_request(int serverSocket, vector<struct pollfd> &fds, map<int, Client> &clients)
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

/* Send a simple HTTP response */
int send_response(int clientSocket)
{
    const char response[] =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 7\r\n"
        "Connection: close\r\n"
        "\r\n"
        "Hello\r\n";

    const size_t length = sizeof(response) - 1;
    size_t sent = 0;

    while (sent < length) {
        ssize_t n = send(clientSocket, response + sent,
                         length - sent, 0);

        if (n < 0 && errno == EINTR)
            continue;

        if (n <= 0) {
            if (n < 0)
                perror("send");
            return -1;
        }

        sent += static_cast<size_t>(n);
    }

    return 0;
}

/* Process events reported by poll */
int process_events(vector<struct pollfd> &fds, int serverSocket, map<int, Client> &clients)
{
    for (size_t i = 0; i < fds.size(); ) {

        short events = fds[i].revents;

        /* Server socket */
        if (fds[i].fd == serverSocket) {

            if (events & (POLLERR | POLLHUP | POLLNVAL)) {
                cerr << "Listening socket error" << endl;
                return -1;
            }

            if (events & POLLIN) {
                if (accept_request(serverSocket, fds, clients) < 0)
                    return -1;
            }

            ++i;
            continue;
        }

        /* Client socket: error */
        if (events & (POLLERR | POLLNVAL)) {
            remove_client(fds, clients, i);
            continue;
        }

        /* Client socket: data available */
        if (events & POLLIN) {
            int fd = fds[i].fd;
            map<int, Client>::iterator it = clients.find(fd);

            if (it != clients.end()) {
                int result = handle_client(it->second);

                if (result == CLIENT_DISCONNECTED || result == CLIENT_ERROR) {
                    remove_client(fds, clients, i);
                    continue;
                }
            }
        }

        /* Client socket: disconnected */
        if (events & POLLHUP) {
            remove_client(fds, clients, i);
            continue;
        }

        ++i;
    }

    return 0;
}

/* Main event loop */
int run_server(int serverSocket)
{
    vector<struct pollfd> fds;
    map<int, Client> clients;

    add_pollfd(fds, serverSocket);

    int status = 0;

    while (true) {
        int ret = poll(&fds[0], fds.size(), -1);

        if (ret < 0) {
            if (errno == EINTR)
                continue;

            perror("poll");
            status = 1;
            break;
        }

        if (process_events(fds, serverSocket, clients) < 0) {
            status = 1;
            break;
        }
    }

    /* Close remaining client sockets */
    for (size_t i = 1; i < fds.size(); ++i)
        close(fds[i].fd);

    return status;
}

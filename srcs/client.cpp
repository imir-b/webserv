
/* Header files */
#include "main.hpp"
#include "client.hpp"

# pragma region Client class implementation

Client::Client(int fd)
    : fd(fd), bytesSent(0)
{
}

Client::~Client()
{
}

int Client::getFd() const
{
    return fd;
}

const std::string& Client::getReadBuffer() const
{
    return readBuffer;
}

const std::string& Client::getWriteBuffer() const
{
    return writeBuffer;
}

size_t Client::getBytesSent() const
{
    return bytesSent;
}

void Client::appendReadBuffer(const char *data, size_t length)
{
    readBuffer.append(data, length);
}

void Client::appendWriteBuffer(const std::string& data)
{
    writeBuffer.append(data);
}

void Client::clearReadBuffer()
{
    readBuffer.clear();
}

void Client::clearWriteBuffer()
{
    writeBuffer.clear();
    bytesSent = 0;
}

void Client::addBytesSent(size_t bytes)
{
    bytesSent += bytes;
}

void Client::resetBytesSent()
{
    bytesSent = 0;
}

#pragma endregion

/* Close and remove a client */
void remove_client(vector<struct pollfd> &fds, map<int, Client> &clients, size_t index)
{
    int fd = fds[index].fd;
    close(fd);
    clients.erase(fd);
    fds.erase(fds.begin() + index);
}

/* Receive data from a client and store it in the client buffer */
int handle_client(Client &client)
{
    char buffer[BUFSIZE];

    while (true) // ajoute une limite pour eviter detre bloque par un client qui monopolise le programme avec des requetes constantes <<<<<-------
    {
        ssize_t bytes = recv(client.getFd(), buffer,
                         sizeof(buffer), 0);

        if (bytes > 0)
        {
            client.appendReadBuffer(buffer, bytes);
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
}


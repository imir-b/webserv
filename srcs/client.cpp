
/* Header files */
#include "main.hpp"
#include "client.hpp"

/* Receive data from a client and store it in the client read buffer */
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
            cout << "Read buffer client (fd " << client.getFd() << "): ";
            cout.write(client.getReadBuffer().data(), client.getReadBuffer().size());
            cout << endl;
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

# pragma region Client class implementation

Client::Client(int fd) : fd(fd), bytesSent(0) {}

Client::~Client(void) {}

int Client::getFd(void) const
{
    return fd;
}

const std::string& Client::getReadBuffer(void) const
{
    return readBuffer;
}

const std::string& Client::getWriteBuffer(void) const
{
    return writeBuffer;
}

size_t Client::getBytesSent(void) const
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

void Client::consumeReadBuffer(size_t length)
{
    readBuffer.erase(0, length);
}

void Client::consumeWriteBuffer(size_t length)
{
    writeBuffer.erase(0, length);
}

void Client::clearReadBuffer(void)
{
    readBuffer.clear();
}

void Client::clearWriteBuffer(void)
{
    writeBuffer.clear();
    bytesSent = 0;
}

void Client::addBytesSent(size_t bytes)
{
    bytesSent += bytes;
}

void Client::resetBytesSent(void)
{
    bytesSent = 0;
}

#pragma endregion

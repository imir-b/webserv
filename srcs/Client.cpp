
/* HPP headers */
#include "../headers/Main.hpp"
#include "../headers/Client.hpp"

/* Constructor */

Client::Client(int fd) : fd(fd), bytesSent(0), closeAfterResponse(false) {}

Client::~Client(void) {}

/* Getters */

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

Request& Client::getRequest()
{
    return request;
}

const Request& Client::getRequest() const
{
    return request;
}

/* Buffer management methods */

bool Client::appendReadBuffer(const char *data, size_t length)
{
    if (readBuffer.size() > MAX_READ_BUFFER_SIZE ||
        length > MAX_READ_BUFFER_SIZE - readBuffer.size())
        return false;

    readBuffer.append(data, length);
    return true;
}

bool Client::appendWriteBuffer(const string& data)
{
    if (writeBuffer.size() > MAX_WRITE_BUFFER_SIZE ||
        data.size() > MAX_WRITE_BUFFER_SIZE - writeBuffer.size())
        return false;

    writeBuffer.append(data);
    return true;
}

void Client::consumeReadBuffer(size_t length)
{
    if (length >= readBuffer.size())
        readBuffer.clear();
    else
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

/* Bytes sent management methods */

void Client::addBytesSent(size_t bytes)
{
    bytesSent += bytes;
}

void Client::resetBytesSent(void)
{
    bytesSent = 0;
}

/* Close after response management methods */

void Client::setCloseAfterResponse(bool close)
{
    closeAfterResponse = close;
}

bool Client::shouldCloseAfterResponse() const
{
    return closeAfterResponse;
}

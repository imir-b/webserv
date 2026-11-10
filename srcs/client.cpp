
/* HPP headers */
#include "main.hpp"
#include "client.hpp"

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

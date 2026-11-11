#pragma once

/* C++ includes */
#include <string>
#include <cstddef>

/* HPP headers */
#include "Request.hpp"

/* Buffer limits */
#define MAX_HEADER_SIZE (16 * 1024)       // 16 KiB
#define MAX_BODY_SIZE (1024 * 1024)       // 1 MiB
#define MAX_READ_BUFFER_SIZE (MAX_HEADER_SIZE + MAX_BODY_SIZE)
#define MAX_WRITE_BUFFER_SIZE (1024 * 1024) // 1 MiB

using namespace std;

class Client
{
    private:
        /* Attributes */
        int fd;
        string readBuffer;
        string writeBuffer;
        size_t bytesSent;
        bool closeAfterResponse;

        /* Request of the client */
        Request request;

    public:
        /* Constructor */
        Client(int fd);
        ~Client();

        /* Getters */
        int getFd() const;
        const string& getReadBuffer() const;
        const string& getWriteBuffer() const;
        size_t getBytesSent() const;
        Request& getRequest();
        const Request& getRequest() const;

        /* Buffer management */
        bool appendReadBuffer(const char *data, size_t length);
        bool appendWriteBuffer(const string& data);
        void consumeReadBuffer(size_t lenght);
        void consumeWriteBuffer(size_t lenght);
        void clearReadBuffer();
        void clearWriteBuffer();

        /* Sending progress */
        void addBytesSent(size_t bytes);
        void resetBytesSent();

        /* Close after response */
        void setCloseAfterResponse(bool close);
        bool shouldCloseAfterResponse() const;
};


#pragma once

#include <string>
#include <cstddef>

#include "request.hpp"

using namespace std;

class Client
{
    private:
        int fd;
        string readBuffer;
        string writeBuffer; // ajoute une limite aux 2 buffers pour eviter d'ecrire a l'infini des requetes enormes <<<---
        size_t bytesSent;
        bool closeAfterResponse;

        Request request;

    public:
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
        void appendReadBuffer(const char *data, size_t length);
        void appendWriteBuffer(const string& data);
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

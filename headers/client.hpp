
#pragma once

#include <string>
#include <cstddef>

using namespace std;

class Client
{
    public:
        Client(int fd);
        ~Client();

        /* Getters */
        int getFd() const;
        const string& getReadBuffer() const;
        const string& getWriteBuffer() const;
        size_t getBytesSent() const;

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

    private:
        int fd;
        string readBuffer;
        string writeBuffer; // ajoute une limite aux 2 buffers pour eviter d'ecrire a l'infini des requetes enormes <<<-----
        size_t bytesSent;
        bool closeAfterResponse; // oublie pas de m implementer boss pour savoir quand on peut suppr la class ou quand on en a encore besoin <<<<<----
};

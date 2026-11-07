
#pragma once

#include <string>
#include <cstddef>

class Client
{
    public:
        Client(int fd);
        ~Client();

        /* Getters */
        int getFd() const;
        const std::string& getReadBuffer() const;
        const std::string& getWriteBuffer() const;
        size_t getBytesSent() const;

        /* Buffer management */
        void appendReadBuffer(const char *data, size_t length);
        void appendWriteBuffer(const std::string& data);
        void clearReadBuffer();
        void clearWriteBuffer();

        /* Sending progress */
        void addBytesSent(size_t bytes);
        void resetBytesSent();

    private:
        int fd;
        std::string readBuffer;
        std::string writeBuffer; // ajoute une limite aux 2 buffers pour eviter d'ecrire a l'infini des requetes enormes <<<-----
        size_t bytesSent;
        bool closeAfterResponse; // oublie pas de m implementer boss pour savoir quand on peut suppr la class ou quand on en a encore besoin <<<<<----
};

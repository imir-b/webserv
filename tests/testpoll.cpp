
#include <iostream>
#include <poll.h>
#include <unistd.h>
#include <cstring>

// POLLIN : des données sont disponibles en lecture.
// POLLOUT : le descripteur est prêt pour une écriture.
// POLLERR : une erreur est signalée.
// POLLHUP : la connexion ou le flux a été fermé.
// POLLNVAL : le descripteur est invalide.

int main()
{
    int pipefd[2];

    if (pipe(pipefd) < 0)
    {
        perror("pipe");
        return 1;
    }

    struct pollfd fds[2] = {
        {pipefd[0], POLLIN, 0},  // Surveiller la lecture
        {pipefd[1], POLLOUT, 0}  // Surveiller l'ecriture
    };

    // 1. POLLOUT : le pipe est pret a recevoir des donnees
    std::cout << "=== Test POLLOUT ===\n";

    int ret = poll(&fds[1], 1, 1000);

    if (ret > 0 && (fds[1].revents & POLLOUT))
        std::cout << "POLLOUT : le pipe est pret pour une ecriture\n";

    // 2. POLLIN : ecrire des donnees dans le pipe
    const char msg[] = "Bonjour poll !";
    write(pipefd[1], msg, sizeof(msg));

    std::cout << "\n=== Test POLLIN ===\n";

    ret = poll(&fds[0], 1, 1000);

    if (ret > 0 && (fds[0].revents & POLLIN))
    {
        std::cout << "POLLIN : des donnees sont disponibles\n";

        char buffer[128] = {0};
        ssize_t n = read(pipefd[0], buffer, sizeof(buffer) - 1);

        if (n > 0)
            std::cout << "Message lu : " << buffer << '\n';
    }
    else if (ret == 0)
    {
        std::cout << "Timeout : aucune donnee disponible\n";
    }
    else if (ret < 0)
    {
        perror("poll");
    }

    close(pipefd[0]);
    close(pipefd[1]);

    return 0;
}

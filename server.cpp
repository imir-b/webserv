
/* LibC includes */
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>

/* C++ includes */
#include <cerrno>
#include <csignal>
#include <vector>
#include <iostream>
#include <string>

using namespace std;

#define PORT 6767
#define BACKLOG 5
#define BUFSIZE 1024


/**
 * ## TO-DO LIST ##
 *
 * 1. GESTION NON BLOQUANTE DES SOCKETS
 *     - Comprendre fcntl(), O_NONBLOCK
 *     - Comprendre EAGAIN et EWOULDBLOCK
 *     - Passer les sockets serveur et clients en non-bloquant
 *     - Gérer les erreurs sans arrêter tout le serveur
 *
 * 2. AMÉLIORER LA BOUCLE POLL()
 *     - Surveiller POLLIN et POLLOUT
 *     - Gérer les lectures et écritures partielles
 *     - Gérer les déconnexions et les timeouts
 *     - Éviter les appels bloquants
 *
 * 3. GESTION DES CLIENTS
 *     - Créer une classe/structure Client
 *     - Ajouter un buffer de réception par client
 *     - Ajouter un buffer d'envoi par client
 *     - Conserver l'état de chaque connexion
 *     - Nettoyer correctement les ressources
 *
 * 4. PARSING DES REQUÊTES HTTP
 *     - Parser la request line (méthode, URI, version)
 *     - Parser les headers HTTP
 *     - Détecter la fin des headers (\r\n\r\n)
 *     - Gérer Content-Length
 *     - Gérer Transfer-Encoding: chunked
 *     - Gérer les requêtes incomplètes ou invalides
 *     - Limiter la taille des requêtes et des bodies
 *
 * 5. CONSTRUCTION DES RÉPONSES HTTP
 *     - Créer une classe HttpResponse
 *     - Générer les status codes (200, 404, 403, 500...)
 *     - Générer les headers (Content-Type, Content-Length...)
 *     - Générer les réponses personnalisées
 *     - Gérer Connection: keep-alive / close
 *
 * 6. GESTION DES ROUTES ET FICHIERS
 *     - Implémenter GET (servir HTML, CSS, JS, images...)
 *     - Implémenter POST (réception de données / uploads)
 *     - Implémenter DELETE (suppression de ressources)
 *     - Gérer les MIME types
 *     - Gérer les redirections HTTP
 *     - Gérer les pages d'erreur personnalisées
 *     - Sécuriser les chemins (éviter les directory traversals)
 *
 * 7. FICHIER DE CONFIGURATION
 *     - Parser un fichier de configuration
 *     - Gérer les ports et adresses d'écoute
 *     - Gérer les server names et les locations
 *     - Gérer les root directories et index files
 *     - Configurer les méthodes HTTP autorisées
 *     - Configurer les limites de taille des bodies
 *     - Gérer plusieurs serveurs virtuels
 *
 * 8. CGI (COMMON GATEWAY INTERFACE)
 *     - Comprendre fork(), execve(), pipe(), dup2()
 *     - Exécuter un script CGI
 *     - Transmettre les données de la requête au CGI
 *     - Récupérer stdout et les headers CGI
 *     - Gérer les erreurs et les timeouts CGI
 *
 * 9. TESTS ET ROBUSTESSE
 *     - Tester avec curl, nc et un navigateur
 *     - Tester plusieurs clients simultanés
 *     - Tester les requêtes fragmentées
 *     - Tester les gros fichiers et les uploads
 *     - Tester les clients lents et les déconnexions
 *     - Vérifier les fuites de FD et de mémoire
 *     - Comparer les comportements avec nginx
 *     - Vérifier la conformité C++98
 *
 */


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
int setup_server_socket()
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

/* Close and remove a client */
void remove_client(vector<struct pollfd> &fds, size_t index)
{
    close(fds[index].fd);
    fds.erase(fds.begin() + index);
}

/* Accept a new connection */
int accept_request(int serverSocket, vector<struct pollfd> &fds)
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

/* Receive data from a client */
int handle_client(int clientSocket)
{
    char buffer[BUFSIZE];

    ssize_t bytesRead = recv(clientSocket, buffer,
                             sizeof(buffer), 0);

    if (bytesRead > 0) {
        cout << "Received data (fd " << clientSocket << "): ";
        cout.write(buffer, bytesRead);
        cout << endl;

        if (send_response(clientSocket) < 0) {
            return -1;
        }
    }
    else if (bytesRead == 0) {
        cout << "Client disconnected (fd "
             << clientSocket << ")." << endl;
    }
    else {
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
            return 0;
        perror("recv");
        return -1;
    }

    return 0;
}

/* Process events reported by poll */
int process_events(vector<struct pollfd> &fds, int serverSocket)
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
                if (accept_request(serverSocket, fds) < 0)
                    return -1;
            }

            ++i;
            continue;
        }

        /* Client socket: error */
        if (events & (POLLERR | POLLNVAL)) {
            remove_client(fds, i);
            continue;
        }

        /* Client socket: data available */
        if (events & POLLIN) {
            if (handle_client(fds[i].fd) < 0) {
                remove_client(fds, i);
                continue;
            }
            remove_client(fds, i);
            continue;
        }

        /* Client socket: disconnected */
        if (events & POLLHUP) {
            remove_client(fds, i);
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

        if (process_events(fds, serverSocket) < 0) {
            status = 1;
            break;
        }
    }

    /* Close remaining client sockets */
    for (size_t i = 1; i < fds.size(); ++i)
        close(fds[i].fd);

    return status;
}

int main()
{
    signal(SIGPIPE, SIG_IGN);

    int serverSocket = setup_server_socket();
    if (serverSocket < 0)
        return 1;

    cout << "Server is listening on port "
         << PORT << "..." << endl;

    int status = run_server(serverSocket);

    close(serverSocket);
    return status;
}


/* HPP headers */
#include "main.hpp"
#include "server.hpp"

int main()
{
    Server server;
    signal(SIGPIPE, SIG_IGN);

    int serverSocket = server.setup();
    if (serverSocket < 0)
        return 1;

    cout << "Server is listening on port "
         << PORT << "..." << endl;

    int status = server.run();

    close(serverSocket);
    
    return status;
}

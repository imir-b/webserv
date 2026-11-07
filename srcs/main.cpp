
#include "main.hpp"


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

#include "Server.hpp"

pollfd g_pfd[2];

int main()
{
    using namespace http;

    g_pfd[0].events = (POLLIN | POLLRDNORM | POLLOUT | POLLWRNORM); // Event that you want to monitor for this fd
    g_pfd[1].events = (POLLIN | POLLRDNORM | POLLOUT | POLLWRNORM); // Event that you want to monitor for this fd


    TcpServer server = TcpServer("127.0.0.1", 8080, 0);
    server.startListen();
    TcpServer server2 = TcpServer("127.0.0.1", 3000, 1);
    server2.startListen();
    poll(g_pfd, 2, 1000);
    server.startListen();
    server2.startListen();
    // TcpServer server2 = TcpServer("127.0.0.1", 3000);
    // server2.startListen();

    return 0;
}
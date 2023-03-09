#include "Server.hpp"

int main()
{
    using namespace http;

    TcpServer server = TcpServer("127.0.0.1", 8080);
    server.startListen();
    // TcpServer server2 = TcpServer("127.0.0.1", 3000);
    // server2.startListen();

    return 0;
}
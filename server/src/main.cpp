#include "HTTPServer.hpp"

int main() {
    std::string dbConnInfo = "host=db port=5432 user=myuser password=mypassword dbname=usersdb";
    
    HttpServer::UPtr server = std::make_unique<HttpServer>(dbConnInfo);

    server->Start(7777);

    return 0;
}

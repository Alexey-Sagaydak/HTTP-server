#ifndef _HTTP_SERVER_HPP_
#define _HTTP_SERVER_HPP_

#include "HttpServer.h"
#include "HttpService.h"
#include "Routers.hpp"
#include <memory>

class HttpServer final {
public:
    using UPtr = std::unique_ptr<HttpServer>;

    explicit HttpServer(const std::string& dbConnInfo);
    
    HttpServer(const HttpServer &) = delete;
    HttpServer(HttpServer &&) = delete;
    
    void Start(int port);

    ~HttpServer();

private:
    std::unique_ptr<hv::HttpServer> _server;
    HttpService _router;
    Database _database;
};

#endif

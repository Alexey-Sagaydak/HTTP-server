#ifndef _ROUTERS_HPP_
#define _ROUTERS_HPP_

#include "HttpService.h"
#include "HashUtils.hpp"
#include "Database.hpp"
#include <unordered_map>

namespace route {
    void RegisterResources(hv::HttpService &router, Database &database);

    void authenticate(const HttpRequest* req, HttpResponse* resp, Database &database, bool* isAuth, User* currentUser);

    std::string base64_decode(const std::string& in);
}

#endif

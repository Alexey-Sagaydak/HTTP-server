#ifndef _ROUTERS_HPP_
#define _ROUTERS_HPP_

#include "HttpService.h"
#include "HashUtils.hpp"
#include <unordered_map>

struct User {
    std::string userId;
    std::string username;
    std::string password;
    bool isAdmin;
};

namespace route
{
    void RegisterResources(hv::HttpService &router, std::unordered_map<std::string, User> &users);
    void authenticate(const HttpRequest* req, HttpResponse* resp, std::unordered_map<std::string, User> &users, bool* isAuth, User* currentUser);
    std::string base64_decode(const std::string& in);
}

#endif
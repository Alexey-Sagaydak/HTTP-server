#ifndef _ROUTERS_HPP_
#define _ROUTERS_HPP_

#include "HttpService.h"
#include <unordered_map>

struct User {
    std::string userId;
    std::string username;
    std::string password;
};

namespace route
{
    void RegisterResources(hv::HttpService &router, std::unordered_map<std::string, User> &users);
}

#endif

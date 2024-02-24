#ifndef _ROUTERS_HPP_
#define _ROUTERS_HPP_

#include "HttpService.h"
#include <unordered_map>

namespace route
{
    void RegisterResources(hv::HttpService &router, std::unordered_map<std::string, nlohmann::json> &users);
}

#endif

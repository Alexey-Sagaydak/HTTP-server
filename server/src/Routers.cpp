#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <mutex>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include "Routers.hpp"

std::mutex usersMutex;

void route::RegisterResources(hv::HttpService &router, std::unordered_map<std::string, User> &users)
{
    router.POST("/user", [&users](HttpRequest *req, HttpResponse *resp)
    {
        std::lock_guard<std::mutex> lock(usersMutex);

        nlohmann::json request;
        nlohmann::json response;
        HashUtils hashUtils;
        User newUser;

        try
        {
            request = nlohmann::json::parse(req->body);
            newUser.username = request["username"];
            hashUtils.computeHash(request["password"], newUser.password);
            newUser.isAdmin = request["isAdmin"];
        }
        catch(const std::exception& e)
        {
            response["error"] = "Invalid JSON";
            resp->SetBody(response.dump());
            resp->content_type = APPLICATION_JSON;
            return 400;
        }

        boost::uuids::uuid uuid = boost::uuids::random_generator()();
        newUser.userId = boost::uuids::to_string(uuid);

        // Добавляем пользователя в список
        users[newUser.userId] = newUser;

        response["userId"] = newUser.userId;
        response["username"] = newUser.username;
        response["password"] = newUser.password;
        response["isAdmin"] = newUser.isAdmin;
        response["msg"] = "User added successfully";
        resp->SetBody(response.dump());
        resp->content_type = APPLICATION_JSON;

        return 200;
    });

    router.GET("/user/{userId}", [&users](HttpRequest *req, HttpResponse *resp)
    {
        std::lock_guard<std::mutex> lock(usersMutex);
        
        std::string userId = req->query_params["userId"];

        // Проверяем, есть ли такой пользователь в списке
        auto it = users.find(userId);
        if (it != users.end())
        {
            nlohmann::json response;
            response["userId"] = it->second.userId;
            response["username"] = it->second.username;
            response["password"] = it->second.password;
            response["isAdmin"] = it->second.isAdmin;

            resp->SetBody(response.dump());
            resp->content_type = APPLICATION_JSON;

            return 200;
        }
        else
        {
            resp->SetBody("User not found");
            resp->status_code = http_status::HTTP_STATUS_NOT_FOUND;
            resp->content_type = TEXT_PLAIN;

            return 404;
        }
    });

    router.GET("/users", [&users](HttpRequest *req, HttpResponse *resp)
    {
        std::lock_guard<std::mutex> lock(usersMutex);
        
        nlohmann::json response;
        for (const auto &user : users)
        {
            nlohmann::json userData;
            userData["userId"] = user.second.userId;
            userData["username"] = user.second.username;
            userData["password"] = user.second.password;
            userData["isAdmin"] = user.second.isAdmin;

            response[user.first] = userData;
        }

        resp->SetBody(response.dump());
        resp->content_type = APPLICATION_JSON;

        return 200;
    });

    router.Delete("/user/{userId}", [&users](HttpRequest *req, HttpResponse *resp)
    {
        std::lock_guard<std::mutex> lock(usersMutex);
        bool isAuth;
        std::string userId = req->query_params["userId"];

        User currentUser;
        authenticate(req, resp, users, &isAuth, &currentUser);

        if (!isAuth) {
            resp->status_code = http_status::HTTP_STATUS_UNAUTHORIZED;
            resp->SetBody("User is not authorized");
            return 401;
        }

        if (!currentUser.isAdmin && currentUser.userId != userId){
            resp->status_code = http_status::HTTP_STATUS_FORBIDDEN;
            return 403;
        }

        // Проверяем, есть ли такой пользователь в списке
        auto it = users.find(userId);
        if (it != users.end())
        {
            users.erase(it);

            resp->SetBody("Done");
            resp->content_type = TEXT_PLAIN;
            resp->status_code = http_status::HTTP_STATUS_OK;

            return 200;
        }
        else
        {
            resp->SetBody("User not found");
            resp->content_type = TEXT_PLAIN;
            resp->status_code = http_status::HTTP_STATUS_NOT_FOUND;

            return 404;
        }
    });

    router.PUT("/user/{userId}", [&users](HttpRequest *req, HttpResponse *resp)
    {
        std::lock_guard<std::mutex> lock(usersMutex);
        HashUtils hashUtils;

        bool isAuth;
        std::string userId = req->query_params["userId"];
        std::string hashedPassword;

        User currentUser;
        authenticate(req, resp, users, &isAuth, &currentUser);

        if (!isAuth) {
            resp->status_code = http_status::HTTP_STATUS_UNAUTHORIZED;
            resp->SetBody("User is not authorized");
            return 401;
        }

        if (!currentUser.isAdmin && currentUser.userId != userId){
            resp->status_code = http_status::HTTP_STATUS_FORBIDDEN;
            return 403;
        }

        // Проверяем, существует ли пользователь с данным ID
        auto it = users.find(userId);
        if (it != users.end())
        {
            // Заменяем данные пользователя новыми данными из запроса
            nlohmann::json requestData = nlohmann::json::parse(req->body);
            it->second.username = requestData["username"];
            hashUtils.computeHash(requestData["password"], hashedPassword);
            it->second.password = hashedPassword;
            it->second.isAdmin = requestData["isAdmin"];

            resp->SetBody("User data updated successfully");
            resp->status_code = http_status::HTTP_STATUS_OK;
            resp->content_type = TEXT_PLAIN;

            return 200;
        }
        else
        {
            resp->SetBody("User not found");
            resp->status_code = http_status::HTTP_STATUS_NOT_FOUND;
            resp->content_type = TEXT_PLAIN;

            return 404;
        }
    });
}

void route::authenticate(const HttpRequest* req, HttpResponse* resp, std::unordered_map<std::string, User> &users, bool* isAuth, User* currentUser) {
    HashUtils hashUtils;
    std::string hashedPassword;
    auto authHeader = req->headers.find("Authorization");

    if (authHeader == req->headers.end()) {
        resp->status_code = HTTP_STATUS_UNAUTHORIZED;
        resp->SetHeader("WWW-Authenticate", "Basic realm=\"Authentication Required\"");
        resp->SetBody("Unauthorized access");
        resp->content_type = TEXT_PLAIN;
        *isAuth = false;
        return;
    }

    std::string authStr = authHeader->second;
    if (!boost::starts_with(authStr, "Basic ")) {
        resp->status_code = HTTP_STATUS_UNAUTHORIZED;
        resp->SetBody("Invalid Authorization header");
        resp->content_type = TEXT_PLAIN;
        *isAuth = false;
        return;
    }

    authStr = authStr.substr(6); // Remove "Basic " prefix
    std::string decoded = base64_decode(authStr);

    std::istringstream iss(decoded);
    std::string username, password;
    std::getline(iss, username, ':');
    std::getline(iss, password);

    auto userIt = std::find_if(users.begin(), users.end(), [&](const auto& pair) {
        return pair.second.username == username;
    });

    hashUtils.computeHash(password, hashedPassword);
    if (userIt == users.end() || userIt->second.password != hashedPassword) {
        resp->status_code = HTTP_STATUS_UNAUTHORIZED;
        resp->SetHeader("WWW-Authenticate", "Basic realm=\"Authentication Required\"");
        resp->SetBody("Unauthorized access");
        resp->content_type = TEXT_PLAIN;
        *isAuth = false;
        return;
    }

    *currentUser = userIt->second;
    *isAuth = true;
}

std::string route::base64_decode(const std::string& in) {
    std::string out;
    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;
    int val = 0, valb = -8;
    for (unsigned char c : in) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}
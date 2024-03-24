#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <mutex>
#include "Routers.hpp"

std::mutex usersMutex;

void route::RegisterResources(hv::HttpService &router, std::unordered_map<std::string, User> &users)
{
    router.POST("/user", [&users](HttpRequest *req, HttpResponse *resp)
    {
        std::lock_guard<std::mutex> lock(usersMutex);

        nlohmann::json request;
        nlohmann::json response;
        User newUser;

        try
        {
            request = nlohmann::json::parse(req->body);
            // Получаем данные пользователя из запроса и записываем в структуру User
            newUser.username = request["username"];
            newUser.password = request["password"];
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

            resp->SetBody(response.dump());
            resp->content_type = APPLICATION_JSON;

            return 200;
        }
        else
        {
            resp->SetBody("User not found");
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

            response[user.first] = userData;
        }

        resp->SetBody(response.dump());
        resp->content_type = APPLICATION_JSON;

        return 200;
    });

    router.Delete("/user/{userId}", [&users](HttpRequest *req, HttpResponse *resp)
    {
        std::lock_guard<std::mutex> lock(usersMutex);

        std::string userId = req->query_params["userId"];

        // Проверяем, есть ли такой пользователь в списке
        auto it = users.find(userId);
        if (it != users.end())
        {
            users.erase(it);

            resp->SetBody("");
            resp->content_type = TEXT_PLAIN;

            return 200;
        }
        else
        {
            resp->SetBody("User not found");
            resp->content_type = TEXT_PLAIN;

            return 404;
        }
    });
}

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <mutex>
#include "Routers.hpp"

std::mutex usersMutex;

void route::RegisterResources(hv::HttpService &router, std::unordered_map<std::string, nlohmann::json> &users)
{
    router.POST("/user", [&users](HttpRequest *req, HttpResponse *resp)
    {
        std::lock_guard<std::mutex> lock(usersMutex);

        nlohmann::json request;
        nlohmann::json response;
        std::string userId;
        nlohmann::json userData;

        try
        {
            request = nlohmann::json::parse(req->body);
            // Получаем данные пользователя из запроса
            userData = request["userData"];
        }
        catch(const std::exception& e)
        {
            response["error"] = "Invalid JSON";
            resp->SetBody(response.dump());
            resp->content_type = APPLICATION_JSON;
            return 400;
        }

        boost::uuids::uuid uuid = boost::uuids::random_generator()();
        userId = boost::uuids::to_string(uuid);

        // Добавляем пользователя в список
        users[userId] = userData;

        response["userId"] = userId; // Отправляем новый id в ответе
        response["userData"] = userData; // Возвращаем всю информацию о пользователе
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
            nlohmann::json response = it->second;
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
            response[user.first] = user.second;
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

#include "Routers.hpp"

void route::RegisterResources(hv::HttpService &router, std::unordered_map<std::string, nlohmann::json> &users)
{
    router.POST("/user", [&users](HttpRequest *req, HttpResponse *resp)
    {
        nlohmann::json request;
        nlohmann::json response;

        try
        {
            request = nlohmann::json::parse(req->body);
        }
        catch(const std::exception& e)
        {
            response["error"] = "Invalid JSON";
            resp->SetBody(response.dump());
            resp->content_type = APPLICATION_JSON;
            return 400;
        }

        // Получаем данные пользователя из запроса
        std::string userId = request["userId"];
        nlohmann::json userData = request["userData"];

        // Добавляем пользователя в список
        users[userId] = userData;

        response["msg"] = "User added successfully";
        resp->SetBody(response.dump());
        resp->content_type = APPLICATION_JSON;

        return 200;
    });

    router.GET("/user/{userId}", [&users](HttpRequest *req, HttpResponse *resp)
    {
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

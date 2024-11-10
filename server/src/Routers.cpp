#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <mutex>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include "Routers.hpp"
#include "HashUtils.hpp"
#include "Database.hpp"

std::mutex usersMutex;

void route::RegisterResources(hv::HttpService &router, Database &database) {
    // POST /user - добавление нового пользователя
    router.POST("/user", [&database](HttpRequest *req, HttpResponse *resp) {
        std::lock_guard<std::mutex> lock(usersMutex);
        nlohmann::json request, response;
        User newUser;
        HashUtils hashUtils;

        try {
            std::cout << req->body;
            request = nlohmann::json::parse(req->body);
            newUser.username = request["username"];
            hashUtils.computeHash(request["password"], newUser.password);
            newUser.isAdmin = request["isAdmin"];
        } catch (const std::exception&) {
            response["error"] = "Invalid JSON";
            resp->SetBody(response.dump());
            resp->content_type = APPLICATION_JSON;
            return 400;
        }

        boost::uuids::uuid uuid = boost::uuids::random_generator()();
        newUser.userId = boost::uuids::to_string(uuid);

        if (database.addUser(newUser)) {
            response["userId"] = newUser.userId;
            response["username"] = newUser.username;
            response["password"] = newUser.password;
            response["isAdmin"] = newUser.isAdmin;
            response["msg"] = "User added successfully";
            resp->SetBody(response.dump());
            resp->content_type = APPLICATION_JSON;
            return 200;
        } else {
            response["error"] = "Failed to add user";
            resp->SetBody(response.dump());
            resp->content_type = APPLICATION_JSON;
            return 500;
        }
    });

    // GET /user/{userId} - получение пользователя по ID
    router.GET("/user/{userId}", [&database](HttpRequest *req, HttpResponse *resp) {
        std::lock_guard<std::mutex> lock(usersMutex);
        nlohmann::json response;
        std::string userId = req->query_params["userId"];
        User user;

        if (database.getUser(userId, user)) {
            nlohmann::json response;
            response["userId"] = user.userId;
            response["username"] = user.username;
            response["password"] = user.password;
            response["isAdmin"] = user.isAdmin;

            resp->SetBody(response.dump());
            resp->content_type = APPLICATION_JSON;

            return 200;
        } else {
            response["error"] = "User not found";
            resp->SetBody(response.dump());
            resp->content_type = APPLICATION_JSON;
            return 404;
        }
    });

    // GET /users - получение всех пользователей
    router.GET("/users", [&database](HttpRequest *req, HttpResponse *resp) {
        std::lock_guard<std::mutex> lock(usersMutex);
        nlohmann::json response;
        std::vector<User> users = database.getAllUsers();

        for (const auto &user : users) {
            nlohmann::json userData;
            userData["userId"] = user.userId;
            userData["username"] = user.username;
            userData["password"] = user.password;
            userData["isAdmin"] = user.isAdmin;

            response[user.userId] = userData;
        }

        resp->SetBody(response.dump());
        resp->content_type = APPLICATION_JSON;
        return 200;
    });

    // DELETE /user/{userId} - удаление пользователя по ID
    router.Delete("/user/{userId}", [&database](HttpRequest *req, HttpResponse *resp) {
        std::lock_guard<std::mutex> lock(usersMutex);
        nlohmann::json response;
        std::string userId = req->query_params["userId"];
        bool isAuth;
        User currentUser;

        authenticate(req, resp, database, &isAuth, &currentUser);

        if (!isAuth) {
            resp->status_code = http_status::HTTP_STATUS_UNAUTHORIZED;
            response["error"] = "User is not authorized";
            resp->SetBody(response.dump());
            return 401;
        }

        if (!currentUser.isAdmin && currentUser.userId != userId) {
            resp->status_code = http_status::HTTP_STATUS_FORBIDDEN;
            response["error"] = "Forbidden action";
            resp->SetBody(response.dump());
            return 403;
        }

        if (database.deleteUser(userId)) {
            response["msg"] = "User deleted successfully";
            resp->SetBody(response.dump());
            return 200;
        } else {
            response["error"] = "User not found";
            resp->SetBody(response.dump());
            return 404;
        }
    });

    // PUT /user/{userId} - обновление данных пользователя по ID
    router.PUT("/user/{userId}", [&database](HttpRequest *req, HttpResponse *resp) {
        std::lock_guard<std::mutex> lock(usersMutex);
        nlohmann::json response, requestData;
        HashUtils hashUtils;
        std::string hashedPassword;
        std::string userId = req->query_params["userId"];
        bool isAuth;
        User currentUser, updatedUser;

        authenticate(req, resp, database, &isAuth, &currentUser);

        if (!isAuth) {
            resp->status_code = http_status::HTTP_STATUS_UNAUTHORIZED;
            response["error"] = "User is not authorized";
            resp->SetBody(response.dump());
            return 401;
        }

        if (!currentUser.isAdmin && currentUser.userId != userId) {
            resp->status_code = http_status::HTTP_STATUS_FORBIDDEN;
            response["error"] = "Forbidden action";
            resp->SetBody(response.dump());
            return 403;
        }

        try {
            requestData = nlohmann::json::parse(req->body);
            updatedUser = currentUser;
            updatedUser.username = requestData["username"];
            hashUtils.computeHash(requestData["password"], updatedUser.password);
            updatedUser.isAdmin = requestData["isAdmin"];
            updatedUser.userId = userId;
        } catch (const std::exception&) {
            response["error"] = "Invalid JSON";
            resp->SetBody(response.dump());
            return 400;
        }

        if (database.updateUser(updatedUser)) {
            response["msg"] = "User data updated successfully";
            resp->SetBody(response.dump());
            return 200;
        } else {
            response["error"] = "Failed to update user data";
            resp->SetBody(response.dump());
            return 500;
        }
        return 404;
    });
}

// Реализация функции аутентификации пользователя
void route::authenticate(const HttpRequest* req, HttpResponse* resp, Database &database, bool* isAuth, User* currentUser) {
    HashUtils hashUtils;
    std::string hashedPassword;
    auto authHeader = req->headers.find("Authorization");

    if (authHeader == req->headers.end()) {
        resp->status_code = HTTP_STATUS_UNAUTHORIZED;
        resp->SetHeader("WWW-Authenticate", "Basic realm=\"Authentication Required\"");
        resp->SetBody("Unauthorized access");
        *isAuth = false;
        return;
    }

    std::string authStr = authHeader->second;
    if (!boost::starts_with(authStr, "Basic ")) {
        resp->status_code = HTTP_STATUS_UNAUTHORIZED;
        resp->SetBody("Invalid Authorization header");
        *isAuth = false;
        return;
    }

    authStr = authStr.substr(6); // Удаление префикса "Basic "
    std::string decoded = base64_decode(authStr);

    std::istringstream iss(decoded);
    std::string username, password;
    std::getline(iss, username, ':');
    std::getline(iss, password);

    User user;
    if (database.getUserByUsername(username, user) && hashUtils.computeHash(password, hashedPassword) && user.password == hashedPassword) {
        *currentUser = user;
        *isAuth = true;
    } else {
        resp->status_code = HTTP_STATUS_UNAUTHORIZED;
        resp->SetHeader("WWW-Authenticate", "Basic realm=\"Authentication Required\"");
        resp->SetBody("Unauthorized access");
        *isAuth = false;
    }
}

// Реализация функции для декодирования Base64
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

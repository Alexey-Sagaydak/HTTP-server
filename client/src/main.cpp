#include "HttpClient.h"
#include <iostream>
#include <string>

using json = nlohmann::json;

enum class TextColor {
    BLACK,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
    GRAY,
    LIGHT_RED,
    LIGHT_GREEN,
    LIGHT_YELLOW,
    LIGHT_BLUE,
    LIGHT_MAGENTA,
    LIGHT_CYAN,
    LIGHT_GRAY,
    DARK_RED,
    DARK_GREEN,
    DARK_YELLOW,
    DARK_BLUE,
    DARK_MAGENTA,
    DARK_CYAN,
    DARK_GRAY,
    RESET
};

const std::string base64_chars =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

void printMenu();
void addUser(hv::HttpClient& httpClient);
void printUser(const std::string& jsonStr);
void printUser(json jsonData);
void setTextColor(TextColor color);
void getAllUsersInfo(hv::HttpClient& httpClient);
void getUserById(hv::HttpClient& httpClient);
void deleteUserById(hv::HttpClient& httpClient);
std::string getBasicAuthHeader();
std::string base64_encode(const std::string &input);

int main() {
    std::string serverAddress = "0.0.0.0:7777";

    hv::HttpClient httpClient(serverAddress.c_str());

    int choice;
    do {
        printMenu();
        std::cin >> choice;

        switch (choice) {
            case 1:
                addUser(httpClient);
                break;
            case 2:
                getAllUsersInfo(httpClient);
                break;
            case 3:
                getUserById(httpClient);
                break;
            case 4:
                deleteUserById(httpClient);
                break;
            case 5:
                break;
            case 6:
                std::cout << "Exiting..." << std::endl;
                break;
            default:
                setTextColor(TextColor::RED);
                std::cout << "Invalid choice. Please enter a valid option." << std::endl;
                setTextColor(TextColor::WHITE);
                break;
        }
    } while (choice != 5);

    return 0;
}

void deleteUserById(hv::HttpClient& httpClient) {
    std::string id;
    HttpRequest req;
    req.method = HTTP_DELETE;

    std::cout << "Enter user ID: ";
    std::cin >> id;

    req.url = "/user/" + id;
    req.SetHeader("Authorization", getBasicAuthHeader());

    HttpResponse resp;
    int ret = httpClient.send(&req, &resp);

    if (ret != 0) {
        setTextColor(TextColor::RED);
        std::cerr << "Failed to delete user. Error: " << http_client_strerror(ret) << std::endl;
        setTextColor(TextColor::WHITE);
        return;
    }

    if (resp.status_code == http_status::HTTP_STATUS_FORBIDDEN){
        setTextColor(TextColor::RED);
        std::cerr << "Access denied." << std::endl;
        setTextColor(TextColor::WHITE);
        return;
    }

    if (resp.status_code == http_status::HTTP_STATUS_UNAUTHORIZED){
        setTextColor(TextColor::RED);
        std::cerr << "Unauthorized." << std::endl;
        setTextColor(TextColor::WHITE);
        return;
    }

    if (resp.status_code == http_status::HTTP_STATUS_OK){
        setTextColor(TextColor::GREEN);
        std::cerr << "Done." << std::endl;
        setTextColor(TextColor::WHITE);
        return;
    }
}

void printMenu() {
    setTextColor(TextColor::YELLOW);
    std::cout << "==== Menu ====" << std::endl;
    std::cout << "1. Add new user" << std::endl;
    std::cout << "2. Get all users" << std::endl;
    std::cout << "3. Get user by ID" << std::endl;
    std::cout << "4. Delete user by ID" << std::endl;
    std::cout << "5. Edit user by ID" << std::endl;
    std::cout << "6. Exit" << std::endl;
    setTextColor(TextColor::WHITE);
    std::cout << "Enter your choice: ";
}

void getAllUsersInfo(hv::HttpClient& httpClient) {
    HttpRequest req;
    req.method = HTTP_GET;
    req.url = "/users";

    HttpResponse resp;
    int ret = httpClient.send(&req, &resp);

    if (ret != 0) {
        setTextColor(TextColor::RED);
        std::cerr << "Failed to get users information. Error: " << http_client_strerror(ret) << std::endl;
        setTextColor(TextColor::WHITE);
        return;
    }

    try {
        json responseData = json::parse(resp.body);

        for (auto it = responseData.begin(); it != responseData.end(); ++it) {
            std::string userId = it.key();
            json userData = it.value();

            printUser(userData);
            std::cout << std::endl;
        }
    } catch (const std::exception& e) {
        setTextColor(TextColor::RED);
        std::cerr << "Error parsing JSON response: " << e.what() << std::endl;
        setTextColor(TextColor::WHITE);
        return;
    }
}

void getUserById(hv::HttpClient& httpClient){
    std::string id;
    HttpRequest req;
    req.method = HTTP_GET;

    std::cout << "Enter user ID: ";
    std::cin >> id;

    req.url = "/user/" + id;

    HttpResponse resp;
    int ret = httpClient.send(&req, &resp);

    if (ret != 0) {
        setTextColor(TextColor::RED);
        std::cerr << "Failed to get users information. Error: " << http_client_strerror(ret) << std::endl;
        setTextColor(TextColor::WHITE);
        return;
    }

    if (resp.status_code == http_status::HTTP_STATUS_NOT_FOUND){
        setTextColor(TextColor::RED);
        std::cerr << "User wasn't found." << std::endl;
        setTextColor(TextColor::WHITE);
        return;
    }

    try {
        json responseData = json::parse(resp.body);
        printUser(responseData);
    } catch (const std::exception& e) {
        setTextColor(TextColor::RED);
        std::cerr << "Error parsing JSON response: " << e.what() << std::endl;
        setTextColor(TextColor::WHITE);
        return;
    }
}

void addUser(hv::HttpClient& httpClient) {
    std::string username, password;
    bool isAdmin;

    std::cout << "Enter username: ";
    std::cin >> username;
    std::cout << "Enter password: ";
    std::cin >> password;
    std::cout << "Is admin? (1 for yes, 0 for no): ";
    std::cin >> isAdmin;

    // Создаем JSON для отправки на сервер
    nlohmann::json requestData;
    requestData["username"] = username;
    requestData["password"] = password;
    requestData["isAdmin"] = isAdmin;

    // Создаем объект HttpRequest и добавляем заголовок Content-Type
    HttpRequest req;
    req.method = HTTP_POST;
    req.url = "/user";
    req.content_type = APPLICATION_JSON;
    req.body = requestData.dump();

    // Отправляем запрос на сервер
    HttpResponse resp;
    int ret = httpClient.send(&req, &resp);

    if (ret != 0) {
        setTextColor(TextColor::RED);
        std::cerr << "Failed to add user. Error: " << http_client_strerror(ret) << std::endl;
        setTextColor(TextColor::WHITE);
    } else {
        setTextColor(TextColor::GREEN);
        std::cout << "User added successfully!" << std::endl;
        setTextColor(TextColor::WHITE);
        std::cout << "Server response: " << std::endl;

        printUser(resp.body);
    }
}

void printUser(const std::string& jsonStr) {
    try {
        json jsonData = json::parse(jsonStr);
        printUser(jsonData);
    } catch (const std::exception& e) {
        setTextColor(TextColor::RED);
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        setTextColor(TextColor::WHITE);
    }
}

void printUser(json jsonData) {
    try {
        setTextColor(TextColor::GRAY);
        
        std::cout << "ID: " << jsonData["userId"] << std::endl;
        std::cout << "Username: " << jsonData["username"] << std::endl;
        std::cout << "Password: " << jsonData["password"] << std::endl;
        std::cout << "Is Admin: " << (jsonData["isAdmin"] ? "Yes" : "No") << std::endl;

        setTextColor(TextColor::WHITE);
    } catch (const std::exception& e) {
        setTextColor(TextColor::RED);
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        setTextColor(TextColor::WHITE);
    }
}

std::string getBasicAuthHeader() {
    std::string username, password;
    
    std::cout << "Enter your username: ";
    std::cin >> username;

    std::cout << "Enter your password: ";
    std::cin >> password;

    std::stringstream authData;
    authData << username << ":" << password;
    std::string encodedAuthData = base64_encode(authData.str());

    std::stringstream authHeader;
    authHeader << "Basic " << encodedAuthData;

    return authHeader.str();
}

void setTextColor(TextColor color) {
    switch (color) {
        case TextColor::BLACK:
            std::cout << "\033[30m";
            break;
        case TextColor::RED:
            std::cout << "\033[31m";
            break;
        case TextColor::GREEN:
            std::cout << "\033[32m";
            break;
        case TextColor::YELLOW:
            std::cout << "\033[33m";
            break;
        case TextColor::BLUE:
            std::cout << "\033[34m";
            break;
        case TextColor::MAGENTA:
            std::cout << "\033[35m";
            break;
        case TextColor::CYAN:
            std::cout << "\033[36m";
            break;
        case TextColor::WHITE:
            std::cout << "\033[37m";
            break;
        case TextColor::GRAY:
            std::cout << "\033[90m";
            break;
        case TextColor::LIGHT_RED:
            std::cout << "\033[91m";
            break;
        case TextColor::LIGHT_GREEN:
            std::cout << "\033[92m";
            break;
        case TextColor::LIGHT_YELLOW:
            std::cout << "\033[93m";
            break;
        case TextColor::LIGHT_BLUE:
            std::cout << "\033[94m";
            break;
        case TextColor::LIGHT_MAGENTA:
            std::cout << "\033[95m";
            break;
        case TextColor::LIGHT_CYAN:
            std::cout << "\033[96m";
            break;
        case TextColor::LIGHT_GRAY:
            std::cout << "\033[97m";
            break;
        case TextColor::DARK_RED:
            std::cout << "\033[31;1m";
            break;
        case TextColor::DARK_GREEN:
            std::cout << "\033[32;1m";
            break;
        case TextColor::DARK_YELLOW:
            std::cout << "\033[33;1m";
            break;
        case TextColor::DARK_BLUE:
            std::cout << "\033[34;1m";
            break;
        case TextColor::DARK_MAGENTA:
            std::cout << "\033[35;1m";
            break;
        case TextColor::DARK_CYAN:
            std::cout << "\033[36;1m";
            break;
        case TextColor::DARK_GRAY:
            std::cout << "\033[90m";
            break;
        case TextColor::RESET:
            std::cout << "\033[0m";
            break;
        default:
            std::cout << "\033[0m";
            break;
    }
}

std::string base64_encode(const std::string &input) {
    std::string encoded;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    for (auto& c : input) {
        char_array_3[i++] = c;
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for(i = 0; i < 4; i++) {
                encoded += base64_chars[char_array_4[i]];
            }
            i = 0;
        }
    }

    if (i) {
        for(j = i; j < 3; j++) {
            char_array_3[j] = '\0';
        }

        char_array_4[0] = ( char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

        for (j = 0; j < i + 1; j++) {
            encoded += base64_chars[char_array_4[j]];
        }

        while((i++ < 3)) {
            encoded += '=';
        }
    }

    return encoded;
}

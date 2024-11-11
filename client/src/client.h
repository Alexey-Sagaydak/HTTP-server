#ifndef CLIENT_H
#define CLIENT_H

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

void printMenu();
void addUser(hv::HttpClient& httpClient);
void printUser(const std::string& jsonStr);
void printUser(json jsonData);
void printWeather(json jsonData);
void setTextColor(TextColor color);
void getAllUsersInfo(hv::HttpClient& httpClient);
void getUserById(hv::HttpClient& httpClient);
void deleteUserById(hv::HttpClient& httpClient);
void editUserById(hv::HttpClient& httpClient);
void getWeather(hv::HttpClient& httpClient);
std::string getBasicAuthHeader();
std::string base64_encode(const std::string &input);

#endif
#ifndef _DATABASE_HPP_
#define _DATABASE_HPP_

#include <pqxx/pqxx>

struct User {
    std::string userId;
    std::string username;
    std::string password;
    bool isAdmin;
};

class Database {
public:
    Database(const std::string& connInfo);
    bool addUser(const User& user);
    bool getUser(const std::string& userId, User& user);
    bool deleteUser(const std::string& userId);
    bool updateUser(const User& user);
    std::vector<User> getAllUsers();
    bool getUserByUsername(const std::string& username, User& user);

private:
    pqxx::connection _conn;
};

#endif

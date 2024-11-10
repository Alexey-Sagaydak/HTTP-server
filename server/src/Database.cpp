#include "Database.hpp"

Database::Database(const std::string& connInfo) : _conn(connInfo) {}

bool Database::addUser(const User& user) {
    try {
        pqxx::work txn(_conn);
        txn.exec_params("INSERT INTO users (userId, username, password, isAdmin) VALUES ($1, $2, $3, $4)",
                        user.userId, user.username, user.password, user.isAdmin);
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Database::getUser(const std::string& userId, User& user) {
    try {
        pqxx::work txn(_conn);
        auto row = txn.exec_params("SELECT userId, username, password, isAdmin FROM users WHERE userId = $1", userId);
        if (row.size() == 1) {
            user.userId = row[0][0].as<std::string>();
            user.username = row[0][1].as<std::string>();
            user.password = row[0][2].as<std::string>();
            user.isAdmin = row[0][3].as<bool>();
            return true;
        }
        return false;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Database::deleteUser(const std::string& userId) {
    try {
        pqxx::work txn(_conn);
        txn.exec_params("DELETE FROM users WHERE userId = $1", userId);
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Database::updateUser(const User& user) {
    try {
        pqxx::work txn(_conn);
        txn.exec_params("UPDATE users SET username = $2, password = $3, isAdmin = $4 WHERE userId = $1",
                        user.userId, user.username, user.password, user.isAdmin);
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<User> Database::getAllUsers() {
    std::vector<User> users;
    try {
        pqxx::work txn(_conn);
        auto rows = txn.exec("SELECT userId, username, password, isAdmin FROM users");
        for (const auto &row : rows) {
            User user;
            user.userId = row[0].as<std::string>();
            user.username = row[1].as<std::string>();
            user.password = row[2].as<std::string>();
            user.isAdmin = row[3].as<bool>();
            users.push_back(user);
        }
    } catch (const std::exception& e) { }
    return users;
}

bool Database::getUserByUsername(const std::string& username, User& user) {
    try {
        pqxx::work txn(_conn);
        auto row = txn.exec_params("SELECT userId, username, password, isAdmin FROM users WHERE username = $1", username);
        if (row.size() == 1) {
            user.userId = row[0][0].as<std::string>();
            user.username = row[0][1].as<std::string>();
            user.password = row[0][2].as<std::string>();
            user.isAdmin = row[0][3].as<bool>();
            return true;
        }
        return false;
    } catch (const std::exception& e) {
        return false;
    }
}

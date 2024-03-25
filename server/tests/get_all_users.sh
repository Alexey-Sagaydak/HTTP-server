#!/bin/bash

# Переменные
ip="0.0.0.0:7777"

# Функция для получения всех пользователей
get_all_users() {
    curl "http://${ip}/users"
}

# Основная часть скрипта
get_all_users

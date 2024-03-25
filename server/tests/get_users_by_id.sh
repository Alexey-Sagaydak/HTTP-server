#!/bin/bash

# Проверка наличия аргументов - идентификаторов пользователей
if [ $# -eq 0 ]; then
    echo "Usage: $0 <user_id1> <user_id2> ..."
    exit 1
fi

# Переменные
ip="0.0.0.0:7777"

# Функция для получения данных пользователей по идентификаторам
get_users_by_id() {
    for user_id in "$@"; do
        curl "http://${ip}/user/${user_id}" &
    done

    wait
}

# Основная часть скрипта
get_users_by_id "$@"

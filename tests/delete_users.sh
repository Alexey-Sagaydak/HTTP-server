#!/bin/bash

# Проверка наличия аргументов - идентификаторов пользователей
if [ $# -eq 0 ]; then
    echo "Usage: $0 <user_id1> <user_id2> ..."
    exit 1
fi

# Переменные
ip="0.0.0.0:7777"

# Функция для удаления пользователей
delete_users() {
    for user_id in "$@"; do
        curl -X DELETE "http://${ip}/user/${user_id}" &
    done

    wait
}

# Основная часть скрипта
delete_users "$@"

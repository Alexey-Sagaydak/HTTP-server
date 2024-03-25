#!/bin/bash

# Проверка наличия аргументов
if [ $# -ne 2 ]; then
    echo "Usage: $0 <num_users> <num_threads>"
    exit 1
fi

# Переменные
ip="0.0.0.0:7777"
num_users=$1
num_threads=$2

# Функция для отправки POST-запросов на добавление пользователей
add_users() {
    local start=$1
    local end=$2

    for ((i=start; i<=end; i++)); do
        user_data="{\"username\": \"user${i}\", \"password\": \"password${i}\", \"isAdmin\": true}"
        curl -X POST -H "Content-Type: application/json" -d "${user_data}" "http://${ip}/user" &
    done

    wait
}

# Определяем количество пользователей на один поток
users_per_thread=$((num_users / num_threads))

# Запускаем потоки
for ((i=0; i<num_threads; i++)); do
    start=$((i * users_per_thread + 1))
    end=$(((i + 1) * users_per_thread))

    # Последний поток может обработать дополнительные пользователей, если их количество не делится на num_threads
    if [ $i -eq $((num_threads - 1)) ]; then
        end=$((num_users))
    fi

    add_users $start $end &
done

wait

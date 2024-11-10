# HTTP-server

Этот проект представляет собой web-сервер с REST API, написанный на языке программирования C++ с использованием библиотеки libhv.

## Функционал

1. **Добавление пользователя**
   - Метод: POST
   - Путь: /user
   - Тело запроса: JSON
   - Описание: Добавляет пользователя в систему.

2. **Получение данных выбранного пользователя**
   - Метод: GET
   - Путь: /user/{userId}
   - Тело ответа: JSON
   - Описание: Возвращает данные выбранного пользователя по его идентификатору.

3. **Получение списка пользователей**
   - Метод: GET
   - Путь: /users
   - Тело ответа: JSON
   - Описание: Возвращает список всех пользователей.

4. **Удаление пользователя**
   - Метод: DELETE
   - Путь: /user/{userId}
   - Тело ответа: Пустое
   - Описание: Удаляет пользователя из системы по его идентификатору.

5. **Редактирование данных пользователя**
   - Метод: PUT
   - Путь: /user/{userId}
   - Тело ответа: Пустое
   - Описание: Редактирует данные пользователя по его идентификатору.

## Сборка и запуск

1. Склонируйте репозиторий:
   ```bash
   git clone <URL_репозитория>
   ```

2. Перейдите в `server/build` (создайте при необходимости):

3. Соберите сервер:
   ```bash
   cmake ..
   make
   ```

4. Запустите контейнеры через Docker Compose:
   ```bash
   sudo docker compose up --build -d
   ```

5. Подключитесь к контейнеру с PostgreSQL
   ```bash
   sudo docker compose exec db psql -U myuser -d usersdb
   ```

6. Создайте таблицу `users` при первом запуске (при последующих запусках внесенные изменения сохранятся):
   ```sql
   CREATE TABLE users (
      userId UUID PRIMARY KEY,
      username VARCHAR(50) NOT NULL,
      password VARCHAR(64) NOT NULL,
      isAdmin BOOLEAN NOT NULL
   );
   ```

   Чтобы убедиться, что таблица создана, выполните команду `\d users`:
   ```
   usersdb=# \d users
                        Table "public.users"
   Column   |         Type          | Collation | Nullable | Default 
   ----------+-----------------------+-----------+----------+---------
   userid   | uuid                  |           | not null | 
   username | character varying(50) |           | not null | 
   password | character varying(64) |           | not null | 
   isadmin  | boolean               |           | not null | 
   Indexes:
      "users_pkey" PRIMARY KEY, btree (userid)
      "users_username_key" UNIQUE CONSTRAINT, btree (username)
   ```

7. Проверим работу сервера, запустив клиента из соответствующей папки build:
   ```bash
   ./libhv-client
   ```

8. Для завершения работы сервера введите команду:
   ```bash
   sudo docker compose down
   ```

## Использование
Меню клиента имеет следующий вид:

```
==== Menu ====
1. Add new user
2. Get all users
3. Get user by ID
4. Delete user by ID
5. Edit user by ID
6. Exit
```

```1. Add new user``` - добавление нового пользователя;

```2. Get all users``` - получение данных о всех пользователях;

```3. Get user by ID``` - получение данных о пользователе по его идентификатору;

```4. Delete user by ID``` - удаление пользователя по его идентификатору; \*

```5. Edit user by ID``` - редактироване данных пользователя по его идентификатору; \*

```6. Exit``` - выход из программы.

> \* - необходима авторизация (администратор может редактировать любое дело, а обычный пользователь только свое).
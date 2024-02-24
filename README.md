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

## Инструменты и технологии

- Язык программирования: C++
- Библиотека: libhv
- Утилита для тестирования: curl

## Использование

1. Клонируйте репозиторий:
   ```bash
   git clone <URL_репозитория>
   ```

2. Перейдите в папку build корневого каталога проекта (создайте при необходимости):
   ```bash
   cd <название_папки>
   ```

3. Соберите проект:
   ```bash
   cmake ..
   make
   ```

4. Запустите сервер:
   ```bash
   ./libhv-http
   ```

5. Проверьте функционал сервера с помощью утилиты curl, выполнив различные HTTP запросы.

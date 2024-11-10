cmake_minimum_required(VERSION 3.0)
project(libhv-http)

set(TARGET_NAME "libhv-http")

set(LIBHV_INCLUDE ${PROJECT_BINARY_DIR}/contrib/libhv/include/hv)

# Поиск OpenSSL
find_package(OpenSSL REQUIRED)

# Поиск libpqxx и libpq
find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)
pkg_check_modules(PQ REQUIRED libpq)

# Добавляем пути к заголовкам
include_directories(${PQXX_INCLUDE_DIRS} ${PQ_INCLUDE_DIRS})

add_subdirectory(${PROJECT_SOURCE_DIR}/contrib)

# Устанавливаем флаги компиляции
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Werror -Wempty-body -Wredundant-move -O2")

# Список исходников
file(GLOB_RECURSE SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h )

set(SOURCE_DIR_HEADER "")
execute_process(
    COMMAND find src -type d
    OUTPUT_VARIABLE SOURCE_DIR_HEADER
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
string(REPLACE "\n" ";" SOURCE_DIR_HEADER ${SOURCE_DIR_HEADER})

# Включение директорий с заголовками
include_directories(${SOURCE_DIR_HEADER})
include_directories(${LIBHV_INCLUDE})

# Создание исполняемого файла
add_executable(${TARGET_NAME} ${SOURCES})

# Линковка с библиотеками
target_link_libraries(${TARGET_NAME} ${OPENSSL_LIBRARIES} ${PQXX_LIBRARIES} ${PQ_LIBRARIES} hv_static)

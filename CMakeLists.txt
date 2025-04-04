cmake_minimum_required(VERSION 3.10)
project(practical_work_4 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Сервер
add_executable(server server.c)

# Клиент
add_executable(client client.c)

# Для Linux добавляем необходимые библиотеки
if(UNIX)
    target_link_libraries(server pthread)
    target_link_libraries(client pthread)
endif()

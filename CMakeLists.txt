cmake_minimum_required(VERSION 3.8)
project(text_finder)

set(CMAKE_CXX_STANDARD 17)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# включаем поиск библиотек с использованием find_package()
# через REQUIRED указываем, что библиотеки обязательны
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Concurrent REQUIRED)


add_executable(text_finder main.cpp mainwindow.h mainwindow.cpp indexer.h indexer.cpp find_function.cpp find_function.h)

#add_subdirectory(tests)
include_directories(gui)
# указываем, какие библиотеки необходимо подключить
target_link_libraries(${PROJECT_NAME} Qt5::Core)
target_link_libraries(${PROJECT_NAME} Qt5::Gui)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} Qt5::Concurrent)

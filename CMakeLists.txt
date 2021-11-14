cmake_minimum_required(VERSION 3.1.0)

project(yet-another-spotify-tray VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets DBus REQUIRED)
find_package(X11 REQUIRED)

add_executable(spotify-tray
    main.cpp
    mainwindow.cpp
    winctl.cpp
)

target_link_libraries(spotify-tray Qt5::Widgets Qt5::DBus ${X11_LIBRARIES})

install(TARGETS spotify-tray DESTINATION bin)
install(FILES spotify-tray.desktop DESTINATION share/applications)

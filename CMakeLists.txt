cmake_minimum_required(VERSION 3.1.0)

project(yet-another-spotify-tray VERSION 1.3.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets DBus LinguistTools REQUIRED)
find_package(X11 REQUIRED)

qt5_add_translation(translations l10n/pl_PL.ts)
file(COPY res/l10n.qrc DESTINATION ${CMAKE_BINARY_DIR}/)

add_executable(spotify-tray
    src/main.cpp
    src/mainwindow.cpp
    src/winctl.cpp
    res/icons.qrc
    ${CMAKE_BINARY_DIR}/l10n.qrc
    ${translations}
)

target_link_libraries(spotify-tray Qt5::Widgets Qt5::DBus ${X11_LIBRARIES})

install(TARGETS spotify-tray DESTINATION bin)
install(FILES xdg/spotify-tray.desktop DESTINATION share/applications)
install(FILES xdg/spotify-tray.svg DESTINATION share/icons/hicolor/scalable/apps)

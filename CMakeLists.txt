cmake_minimum_required(VERSION 3.1.0)

project(yet-another-spotify-tray VERSION 1.1.0 LANGUAGES CXX)

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

qt5_add_translation(translations pl_PL.ts)
file(COPY l10n.qrc DESTINATION ${CMAKE_BINARY_DIR}/)

add_executable(spotify-tray
    main.cpp
    mainwindow.cpp
    winctl.cpp
    icons.qrc
    ${CMAKE_BINARY_DIR}/l10n.qrc
    ${translations}
)

target_link_libraries(spotify-tray Qt5::Widgets Qt5::DBus ${X11_LIBRARIES})

install(TARGETS spotify-tray DESTINATION bin)
install(FILES spotify-tray.desktop DESTINATION share/applications)
install(FILES spotify-tray.svg DESTINATION share/icons/hicolor/scalable/apps)

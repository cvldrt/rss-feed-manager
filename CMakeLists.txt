cmake_minimum_required(VERSION 3.14)

project(httpServer LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network HttpServer Widgets Xml)
qt_standard_project_setup()

add_executable(httpServer
        server.cpp
        include/myserver.h src/myserver.cpp
        include/executor.h src/executor.cpp
        include/rssFeed.h src/rssFeed.cpp
        src/rssFeedManager.cpp
        include/rssFeedManager.h
)

add_executable(httpClient
        client.cpp
        include/requestHandler.h src/requestHandler.cpp
        include/mainWindow.h src/mainWindow.cpp
        src/mainwindow.ui
        include/dialogWindow.h src/dialogWindow.cpp
        src/dialogwindow.ui
        src/dialogWindow.cpp
)

target_link_libraries(httpServer Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::HttpServer)
target_link_libraries(httpClient Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Xml)

include(GNUInstallDirs)
install(TARGETS httpServer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

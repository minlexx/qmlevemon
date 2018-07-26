cmake_minimum_required(VERSION 3.7)

set(QTWEBSERVER_VERSION 0.1.0)

project(QtWebServer LANGUAGES CXX VERSION ${QMLEVEMON_VERSION})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS
    Core
    Network
    Xml
    Sql
    Gui
    REQUIRED
)

add_library(${PROJECT_NAME}
    "tcp/tcpmultithreadedserver.cpp"
    "tcp/tcpserverthread.cpp"
    "http/httprequest.cpp"
    "http/httpstatuscodes.cpp"
    "http/httpwebengine.cpp"
    "http/httpresource.cpp"
    "http/httpiodeviceresource.cpp"
    "misc/log.cpp"
    "misc/logger.cpp"
    "sql/sqlconnectionpool.cpp"
    "html/htmldocument.cpp"
    "util/utilassetsresource.cpp"
    "http/httpresponse.cpp"
    "http/httpheaders.cpp"
    "util/utildataurlcodec.cpp"
    "util/utilformurlcodec.cpp"
    "css/cssdocument.cpp"
    "css/cssruleset.cpp"
    "weblayout.cpp")

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Network
    Qt5::Xml
    Qt5::Sql
    Qt5::Gui
)

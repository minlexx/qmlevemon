cmake_minimum_required(VERSION 3.5)

set(QMLEVEMON_VERSION 0.1.0)

project(QMLEVEMon LANGUAGES CXX VERSION ${QMLEVEMON_VERSION})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 5.7 CONFIG REQUIRED COMPONENTS
    Core
    Network
    Xml
    Sql
    Gui
    Qml
    Quick
)

option(BUILD_TESTING "Build tests (for developers)" OFF)

set(qmlevemon_srcs
    "main.cpp"
    "qmlevemon_app.cpp"
    "periodical_refresher.cpp"
    "db/db.cpp"
    "db/db_sqlite.cpp"
    "db/portrait_cache.cpp"
    "eve_api/eve_sso.cpp"
    "eve_api/eve_api.cpp"
    "eve_api/eve_api_tokens.cpp"
    "models/model_manager.cpp"
    "models/character.cpp"
    "models/character_model.cpp"
    "models/update_timestamps.cpp"
    "qml.qrc"
    "img.qrc"
)

set(qtwebserver_srcs
    "qtwebserver/tcp/tcpmultithreadedserver.cpp"
    "qtwebserver/tcp/tcpserverthread.cpp"
    "qtwebserver/http/httprequest.cpp"
    "qtwebserver/http/httpstatuscodes.cpp"
    "qtwebserver/http/httpwebengine.cpp"
    "qtwebserver/http/httpresource.cpp"
    "qtwebserver/http/httpiodeviceresource.cpp"
    "qtwebserver/misc/log.cpp"
    "qtwebserver/misc/logger.cpp"
    "qtwebserver/sql/sqlconnectionpool.cpp"
    "qtwebserver/html/htmldocument.cpp"
    "qtwebserver/util/utilassetsresource.cpp"
    "qtwebserver/http/httpresponse.cpp"
    "qtwebserver/http/httpheaders.cpp"
    "qtwebserver/util/utildataurlcodec.cpp"
    "qtwebserver/util/utilformurlcodec.cpp"
    "qtwebserver/css/cssdocument.cpp"
    "qtwebserver/css/cssruleset.cpp"
    "qtwebserver/weblayout.cpp")

add_executable(${PROJECT_NAME}
    ${qmlevemon_srcs}
    ${qtwebserver_srcs}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "qtwebserver"
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    QMLEVEMON_VERSION="${QMLEVEMON_VERSION}"
)

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Network
    Qt5::Xml
    Qt5::Sql
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
)

if (BUILD_TESTING)
    message(STATUS "BUILD_TESTING is ON: Will build tests")
    add_subdirectory(tests)
endif()

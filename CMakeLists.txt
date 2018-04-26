cmake_minimum_required(VERSION 3.5)

set(QMLEVEMON_VERSION 0.1.0)

project(QMLEVEMon LANGUAGES CXX VERSION ${QMLEVEMON_VERSION})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include(FeatureSummary)

find_package(Qt5 5.9 CONFIG REQUIRED COMPONENTS
    Core
    Network
    Xml
    Sql
    Gui
    Qml
    Quick
    QuickControls2
)

option(BUILD_TESTING "Build tests (for developers)" OFF)

set(qmlevemon_srcs
    "main.cpp"
    "settings.cpp"
    "qmlevemon_app.cpp"
    "db/db.cpp"
    "db/db_sqlite.cpp"
    "db/portrait_cache.cpp"
    "db/type_icons_provider.cpp"
    "eve_api/eve_sso.cpp"
    "eve_api/eve_api.cpp"
    "eve_api/eve_api_tokens.cpp"
    "models/character.cpp"
    "models/character_clone.cpp"
    "models/character_clones_model.cpp"
    "models/character_implants_group.cpp"
    "models/character_model.cpp"
    "models/character_skill.cpp"
    "models/character_skillgroups_model.cpp"
    "models/character_skillqueue.cpp"
    "models/character_skillqueue_model.cpp"
    "models/eve_location.cpp"
    "models/formulas.cpp"
    "models/invtype.cpp"
    "models/model_manager.cpp"
    "models/skill_group.cpp"
    "models/skill_template.cpp"
    "models/skill_tree_model.cpp"
    "models/update_timestamps.cpp"
    "network/periodical_refresher.cpp"
    "network/refresher/clones.cpp"
    "network/refresher/location.cpp"
    "network/refresher/periodical_refresher_worker.cpp"
    "network/refresher/public_data.cpp"
    "network/refresher/server_status.cpp"
    "network/refresher/skills.cpp"
    "network/refresher/wallet.cpp"
    "utils/str_utils.cpp"
    "qml.qrc"
    "img.qrc"
    "sql.qrc"
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

add_executable(${PROJECT_NAME} WIN32
    ${qmlevemon_srcs}
    ${qtwebserver_srcs}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "qtwebserver"
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    QMLEVEMON_VERSION="${QMLEVEMON_VERSION}"
    QT_DEPRECATED_WARNINGS
    QT_NO_CAST_FROM_ASCII
    QT_NO_CAST_TO_ASCII
    QT_NO_URL_CAST_FROM_STRING
    QT_NO_CAST_FROM_BYTEARRAY
    QT_STRICT_ITERATORS
    QT_NO_SIGNALS_SLOTS_KEYWORDS
    QT_USE_FAST_OPERATOR_PLUS
    QT_USE_QSTRINGBUILDER
)

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Network
    Qt5::Xml
    Qt5::Sql
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickControls2
)

if (BUILD_TESTING)
    message(STATUS "BUILD_TESTING is ON: Will build tests")
    add_subdirectory(tests)
endif()

install(
    TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

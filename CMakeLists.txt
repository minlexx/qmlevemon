cmake_minimum_required(VERSION 3.7)

set(QMLEVEMON_VERSION 0.2.0)

project(QMLEVEMon LANGUAGES CXX VERSION ${QMLEVEMON_VERSION})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FeatureSummary)

option(BUILD_TESTING "Build tests (for developers)" OFF)
option(BUILD_FOR_ANDROID "Build for android" OFF)
if (UNIX)
    option(BUILD_FOR_FLATPAK "Should only be set to ON during flatpak build. Do not set manually!" OFF)
endif()

# In debug builds install executables without stripping rpath
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    set(CMAKE_SKIP_INSTALL_RPATH FALSE)
endif()

set(QT_MIN_VERSION 5.9)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Network
    Xml
    Sql
    Gui
    Qml
    Quick
    QuickControls2
)

# Use Qt5Widgets only for desktop
if (NOT BUILD_FOR_ANDROID)
    find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Widgets)
    # Also use QtDBus in Linux (NOT ANDROID and NOT WINDOWS)
    if (NOT WIN32)
        find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS DBus)
    endif()
else()
    find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS AndroidExtras)
endif()

set(qmlevemon_srcs
    "main.cpp"
    "settings.cpp"
    "qmlevemon_app.cpp"
    "notificationsystem.cpp"
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
    "models/mail_models.cpp"
    "models/model_manager.cpp"
    "models/notification.cpp"
    "models/skill_group.cpp"
    "models/skill_template.cpp"
    "models/skill_tree_model.cpp"
    "models/update_timestamps.cpp"
    "network/periodical_refresher.cpp"
    "network/refresher/clones.cpp"
    "network/refresher/location.cpp"
    "network/refresher/mail.cpp"
    "network/refresher/notifications.cpp"
    "network/refresher/periodical_refresher_worker.cpp"
    "network/refresher/public_data.cpp"
    "network/refresher/resolvers.cpp"
    "network/refresher/server_status.cpp"
    "network/refresher/skills.cpp"
    "network/refresher/wallet.cpp"
    "utils/str_utils.cpp"
    "qml.qrc"
    "img.qrc"
    "sql.qrc"
)

if (WIN32)
    list(APPEND qmlevemon_srcs
        "win32/windows_resources.rc")
endif()

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

if (BUILD_FOR_ANDROID)
    add_library(${PROJECT_NAME} SHARED
        ${qmlevemon_srcs}
        ${qtwebserver_srcs}
    )
else()
    add_executable(${PROJECT_NAME} WIN32
        ${qmlevemon_srcs}
        ${qtwebserver_srcs}
    )
endif()

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
    QT_USE_QSTRINGBUILDER
)

# define ANDROID for android builds (I think qmake does this automatically)
if (BUILD_FOR_ANDROID)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        ANDROID
        __ANDROID__
    )
    set(ANDROID_TOOLCHAIN gcc)
endif()

# Define FLATPAK_BUILD for flatpak build. Enables some icon theme customization
if (BUILD_FOR_FLATPAK)
    target_compile_definitions(${PROJECT_NAME} PRIVATE FLATPAK_BUILD)
endif()

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

# Link Qt5Widgets only for desktop
if (NOT BUILD_FOR_ANDROID)
    target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
else()
    target_link_libraries(${PROJECT_NAME} Qt5::AndroidExtras)
endif()

if (BUILD_TESTING)
    message(STATUS "BUILD_TESTING is ON: Will build tests")
    enable_testing()
    add_subdirectory(tests)
endif()


if (WIN32)
    find_program(MAKENSIS
        NAMES makensis.exe
        DOC "Path to makensis executable (to make installer)"
        HINTS "C:/Program Files (x86)/NSIS" "C:/Program Files/NSIS"
    )
    if (MAKENSIS)
        message(STATUS "Found makensis: ${MAKENSIS}")
        add_custom_target(windows-installer
#            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}\\win32\\qmlevemon_installer.nsi ${CMAKE_CURRENT_BINARY_DIR}
            COMMAND ${MAKENSIS} ${CMAKE_CURRENT_SOURCE_DIR}\\win32\\qmlevemon_installer.nsi
            COMMENT "Build Windows installer using NSIS"
            SOURCES "win32/qmlevemon_installer.nsi"
            VERBATIM)
        add_dependencies(windows-installer ${PROJECT_NAME})
    endif()
endif()


if (UNIX)
    include(GNUInstallDirs)

    # install main executable
    install(
        TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

    # install icons to to /usr/share/icons/hicolor/..x../apps
    install(
        FILES "img/app_icon/32.png"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/32x32/apps"
        RENAME "ru.minlexx.qmlevemon.png")
    install(
        FILES "img/app_icon/48.png"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps"
        RENAME "ru.minlexx.qmlevemon.png")
    install(
        FILES "img/app_icon/64.png"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/64x64/apps"
        RENAME "ru.minlexx.qmlevemon.png")
    install(
        FILES "img/app_icon/128.png"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/128x128/apps"
        RENAME "ru.minlexx.qmlevemon.png")
    install(
        FILES "img/app_icon/256.png"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/256x256/apps"
        RENAME "ru.minlexx.qmlevemon.png")
    install(
        FILES "img/app_icon/512.png"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/512x512/apps"
        RENAME "ru.minlexx.qmlevemon.png")
    install(
        FILES "img/app_icon/ru.minlexx.qmlevemon.svg"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps")

    # install desktop file to /usr/share/applications
    install(FILES "linux/ru.minlexx.qmlevemon.desktop"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications")

    # install app metadata to /usr/share/metainfo
    install(FILES "linux/ru.minlexx.qmlevemon.appdata.xml"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/metainfo")

    # Optionally add make target to build flatpak bundle, if flatpak-builder is installed
    # To use it, run "make flatpak-package"
    find_program(FLATPAK_BUILDER
        NAMES flatpak-builder
        DOC "Path to flatpak-builder executable (optional)"
    )
    find_program(FLATPAK NAMES flatpak DOC "Flatpak main executable")
    if (NOT FLATPAK_BUILDER)
        message(STATUS "Flatpak-builder not found, will not build flatpak package")
    else()
        message(STATUS "Found flatpak: ${FLATPAK}")
        message(STATUS "Found flatpak-builder: ${FLATPAK_BUILDER}")

        add_custom_target(flatpak-package
            COMMAND ${FLATPAK_BUILDER}  --verbose --repo=repo --force-clean flatpak-package ${CMAKE_CURRENT_LIST_DIR}/ru.minlexx.qmlevemon.json
            COMMAND ${FLATPAK} build-bundle repo qmlevemon.flatpak ru.minlexx.qmlevemon
            COMMENT "Build flatpak package"
            SOURCES
                "ru.minlexx.qmlevemon.json"
                "linux/ru.minlexx.qmlevemon.appdata.xml"
                "linux/ru.minlexx.qmlevemon.desktop"
        )
        add_dependencies(flatpak-package ${PROJECT_NAME})
    endif()
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

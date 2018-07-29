TEMPLATE = app
VERSION = 0.2
CONFIG += C++11
QT += core network xml sql gui qml quick quickcontrols2
win32: QT += widgets
unix:!android: QT += widgets dbus
android: {
    QT -= widgets
    QT += androidextras
}

DEFINES += QT_DEPRECATED_WARNINGS QT_DISABLE_DEPRECATED_BEFORE=0x060000
DEFINES += QMLEVEMON_VERSION=\\\"$$VERSION\\\"
DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_URL_CAST_FROM_STRING QT_NO_CAST_FROM_BYTEARRAY \
    QT_STRICT_ITERATORS QT_NO_SIGNALS_SLOTS_KEYWORDS QT_USE_QSTRINGBUILDER

INCLUDEPATH += ./src/ ./src/qtwebserver

SOURCES += \
    ./src/main.cpp \
    ./src/qmlevemon_app.cpp \
    ./src/notificationsystem.cpp \
    ./src/settings.cpp \
    ./src/db/db.cpp \
    ./src/db/db_sqlite.cpp \
    ./src/db/portrait_cache.cpp \
    ./src/db/type_icons_provider.cpp \
    ./src/eve_api/eve_sso.cpp \
    ./src/eve_api/eve_api.cpp \
    ./src/eve_api/eve_api_tokens.cpp \
    ./src/models/character.cpp \
    ./src/models/character_clone.cpp \
    ./src/models/character_clones_model.cpp \
    ./src/models/character_implants_group.cpp \
    ./src/models/character_mail_models.cpp \
    ./src/models/character_model.cpp \
    ./src/models/character_notifications.cpp \
    ./src/models/character_skill.cpp \
    ./src/models/character_skillgroups_model.cpp \
    ./src/models/character_skillqueue.cpp \
    ./src/models/character_skillqueue_model.cpp \
    ./src/models/character_wallet_models.cpp \
    ./src/models/eve_location.cpp \
    ./src/models/formulas.cpp \
    ./src/models/invtype.cpp \
    ./src/models/model_manager.cpp \
    ./src/models/skill_group.cpp \
    ./src/models/skill_template.cpp \
    ./src/models/skill_tree_model.cpp \
    ./src/models/update_timestamps.cpp \
    ./src/network/periodical_refresher.cpp \
    ./src/network/refresher/periodical_refresher_worker.cpp \
    ./src/network/refresher/clones.cpp \
    ./src/network/refresher/location.cpp \
    ./src/network/refresher/mail.cpp \
    ./src/network/refresher/notifications.cpp \
    ./src/network/refresher/public_data.cpp \
    ./src/network/refresher/resolvers.cpp \
    ./src/network/refresher/server_status.cpp \
    ./src/network/refresher/skills.cpp \
    ./src/network/refresher/wallet.cpp \
    ./src/utils/str_utils.cpp \
    ./src/qtwebserver/css/cssdocument.cpp \
    ./src/qtwebserver/css/cssruleset.cpp \
    ./src/qtwebserver/html/htmldocument.cpp \
    ./src/qtwebserver/tcp/tcpmultithreadedserver.cpp \
    ./src/qtwebserver/tcp/tcpserverthread.cpp \
    ./src/qtwebserver/http/httprequest.cpp \
    ./src/qtwebserver/http/httpstatuscodes.cpp \
    ./src/qtwebserver/http/httpwebengine.cpp \
    ./src/qtwebserver/http/httpresource.cpp \
    ./src/qtwebserver/http/httpiodeviceresource.cpp \
    ./src/qtwebserver/misc/log.cpp \
    ./src/qtwebserver/misc/logger.cpp \
    ./src/qtwebserver/sql/sqlconnectionpool.cpp \
    ./src/qtwebserver/util/utilassetsresource.cpp \
    ./src/qtwebserver/http/httpresponse.cpp \
    ./src/qtwebserver/http/httpheaders.cpp \
    ./src/qtwebserver/util/utildataurlcodec.cpp \
    ./src/qtwebserver/util/utilformurlcodec.cpp \
    ./src/qtwebserver/weblayout.cpp

HEADERS += \
    ./src/qmlevemon_app.h \
    ./src/notificationsystem.h \
    ./src/settings.h \
    ./src/db/db.h \
    ./src/db/db_sqlite.h \
    ./src/db/portrait_cache.h \
    ./src/db/type_icons_provider.h \
    ./src/eve_api/eve_api.h \
    ./src/eve_api/eve_api_tokens.h \
    ./src/eve_api/eve_sso.h \
    ./src/models/character.h \
    ./src/models/character_clone.h \
    ./src/models/character_clones_model.h \
    ./src/models/character_implants_group.h \
    ./src/models/character_mail_models.h \
    ./src/models/character_model.h \
    ./src/models/character_notifications.h \
    ./src/models/character_skill.h \
    ./src/models/character_skillgroups_model.h \
    ./src/models/character_skillqueue.h \
    ./src/models/character_skillqueue_model.h \
    ./src/models/character_wallet_models.h \
    ./src/models/common_model_base.h \
    ./src/models/eve_location.h \
    ./src/models/formulas.h \
    ./src/models/invtype.h \
    ./src/models/model_manager.h \
    ./src/models/skill_group.h \
    ./src/models/skill_template.h \
    ./src/models/skill_tree_model.h \
    ./src/models/update_timestamps.h \
    ./src/network/periodical_refresher.h \
    ./src/network/refresher/periodical_refresher_worker.h \
    ./src/utils/str_utils.h \
    ./src/qtwebserver/css/cssdocument.h \
    ./src/qtwebserver/css/cssruleset.h \
    ./src/qtwebserver/html/htmldocument.h \
    ./src/qtwebserver/http/httpheaders.h \
    ./src/qtwebserver/http/httpiodeviceresource.h \
    ./src/qtwebserver/http/httprequest.h \
    ./src/qtwebserver/http/httpresource.h \
    ./src/qtwebserver/http/httpresponse.h \
    ./src/qtwebserver/http/httpstatuscodes.h \
    ./src/qtwebserver/http/httpwebengine.h \
    ./src/qtwebserver/misc/log.h \
    ./src/qtwebserver/misc/logger.h \
    ./src/qtwebserver/misc/threadsafety.h \
    ./src/qtwebserver/sql/sqlconnectionpool.h \
    ./src/qtwebserver/tcp/tcpmultithreadedserver.h \
    ./src/qtwebserver/tcp/tcpresponder.h \
    ./src/qtwebserver/tcp/tcpserverthread.h \
    ./src/qtwebserver/util/utilassetsresource.h \
    ./src/qtwebserver/util/utildataurlcodec.h \
    ./src/qtwebserver/util/utilformurlcodec.h \
    ./src/qtwebserver/weblayout.h \
    ./src/qtwebserver/webwidget.h

RESOURCES += ./src/qml.qrc ./src/img.qrc ./src/sql.qrc

# Windows native embedded resources
win32: HEADERS += ./win32/win32_version.h
win32: RC_FILE = ./win32/windows_resources.rc

OTHER_FILES += \
    ./.gitignore \
    ./CMakeLists.txt \
    ./LICENSE \
    ./README.md \
    ./ru.minlexx.qmlevemon.json

win32: OTHER_FILES += ./win32/windows_resources.rc

android: {
    OTHER_FILES += \
        ./android/src/AndroidManifest.xml \
        ./android/src/src/ru/minlexx/qmlevemon/QMLEVEMonActivity.java
}

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android/src
contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_EXTRA_LIBS = \
        $$PWD/android/arm-linux-androideabi-4.9/libcrypto.so \
        $$PWD/android/arm-linux-androideabi-4.9/libssl.so
}

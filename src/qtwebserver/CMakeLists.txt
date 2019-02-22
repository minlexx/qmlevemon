find_package(Qt5 CONFIG REQUIRED COMPONENTS Core Network Xml Sql Gui)

set(QTWEBSERVER_SRCS
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

add_library(QtWebServer STATIC ${QTWEBSERVER_SRCS})

target_compile_definitions(QtWebServer PRIVATE
    QT_DEPRECATED_WARNINGS
    QT_NO_CAST_FROM_ASCII
    QT_NO_CAST_TO_ASCII
    QT_NO_URL_CAST_FROM_STRING
    QT_NO_CAST_FROM_BYTEARRAY
    QT_STRICT_ITERATORS
    QT_NO_SIGNALS_SLOTS_KEYWORDS
    QT_USE_QSTRINGBUILDER
)

target_link_libraries(QtWebServer PUBLIC
    Qt5::Core
    Qt5::Network
    Qt5::Xml
    Qt5::Sql
    Qt5::Gui
)

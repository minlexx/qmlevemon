find_package(Qt5 5.9 CONFIG REQUIRED COMPONENTS
    Test
)

set(test_qtwebserver_srcs
    "../qtwebserver/tcp/tcpmultithreadedserver.cpp"
    "../qtwebserver/tcp/tcpserverthread.cpp"
    "../qtwebserver/http/httprequest.cpp"
    "../qtwebserver/http/httpstatuscodes.cpp"
    "../qtwebserver/http/httpwebengine.cpp"
    "../qtwebserver/http/httpresource.cpp"
    "../qtwebserver/http/httpiodeviceresource.cpp"
    "../qtwebserver/misc/log.cpp"
    "../qtwebserver/misc/logger.cpp"
    "../qtwebserver/sql/sqlconnectionpool.cpp"
    "../qtwebserver/html/htmldocument.cpp"
    "../qtwebserver/util/utilassetsresource.cpp"
    "../qtwebserver/http/httpresponse.cpp"
    "../qtwebserver/http/httpheaders.cpp"
    "../qtwebserver/util/utildataurlcodec.cpp"
    "../qtwebserver/util/utilformurlcodec.cpp"
    "../qtwebserver/css/cssdocument.cpp"
    "../qtwebserver/css/cssruleset.cpp"
    "../qtwebserver/weblayout.cpp")

add_executable(test_qtwebserver "test_qtwebserver.cpp" ${test_qtwebserver_srcs})
target_include_directories(test_qtwebserver PRIVATE "../qtwebserver")
target_link_libraries(test_qtwebserver
    Qt5::Core
    Qt5::Gui
    Qt5::Network
    Qt5::Sql
    Qt5::Xml
)


#set(test_character_datastream_srcs
#    "test_character_datastream.cpp"
#    "../models/character.cpp"
#    "../models/character_model.cpp"
#    "../models/character_skill.cpp"
#    "../models/model_manager.cpp"
#    "../models/skill_group.cpp"
#    "../models/skill_template.cpp"
#    "../models/skill_tree_model.cpp"
#    "../models/update_timestamps.cpp"
#    "../eve_api/eve_api_tokens.cpp"
#    "../qmlevemon_app.cpp"
#)
#add_executable(test_character_datastream ${test_character_datastream_srcs})
#target_include_directories(test_character_datastream PRIVATE "../")
#target_compile_definitions(test_character_datastream PRIVATE QMLEVEMON_VERSION="${QMLEVEMON_VERSION}")
#target_link_libraries(test_character_datastream
#    Qt5::Core
#    Qt5::Gui
#    Qt5::Sql
#    Qt5::Test
#    Qt5::Qml
#    Qt5::Quick
#)

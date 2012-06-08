QT += core network script
QT -= gui

TARGET = MUD
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
    main.cpp \
    engine/area.cpp \
    engine/character.cpp \
    engine/characterstats.cpp \
    engine/class.cpp \
    engine/commandinterpreter.cpp \
    engine/engine.cpp \
    engine/exit.cpp \
    engine/gameexception.cpp \
    engine/gameobject.cpp \
    engine/gameobjectptr.cpp \
    engine/gameobjectsyncthread.cpp \
    engine/item.cpp \
    engine/player.cpp \
    engine/race.cpp \
    engine/realm.cpp \
    engine/scriptengine.cpp \
    engine/scriptfunction.cpp \
    engine/scriptfunctionmap.cpp \
    engine/scriptwindow.cpp \
    engine/util.cpp \
    engine/commands/closecommand.cpp \
    engine/commands/command.cpp \
    engine/commands/dropcommand.cpp \
    engine/commands/getcommand.cpp \
    engine/commands/givecommand.cpp \
    engine/commands/gocommand.cpp \
    engine/commands/inventorycommand.cpp \
    engine/commands/killcommand.cpp \
    engine/commands/lookcommand.cpp \
    engine/commands/opencommand.cpp \
    engine/commands/quitcommand.cpp \
    engine/commands/saycommand.cpp \
    engine/commands/shoutcommand.cpp \
    engine/commands/slashmecommand.cpp \
    engine/commands/statscommand.cpp \
    engine/commands/talkcommand.cpp \
    engine/commands/tellcommand.cpp \
    engine/commands/whocommand.cpp \
    engine/commands/admin/addcharactercommand.cpp \
    engine/commands/admin/addexitcommand.cpp \
    engine/commands/admin/additemcommand.cpp \
    engine/commands/admin/admincommand.cpp \
    engine/commands/admin/execscriptcommand.cpp \
    engine/commands/admin/getpropcommand.cpp \
    engine/commands/admin/gettriggercommand.cpp \
    engine/commands/admin/removeexitcommand.cpp \
    engine/commands/admin/removeitemcommand.cpp \
    engine/commands/admin/setpropcommand.cpp \
    engine/commands/admin/settriggercommand.cpp \
    engine/commands/admin/stopservercommand.cpp \
    engine/commands/admin/unsettriggercommand.cpp \
    interface/httpserver.cpp \
    interface/session.cpp \
    interface/telnetserver.cpp \
    interface/websocketserver.cpp \
    ../3rdparty/qjson/json_driver.cc \
    ../3rdparty/qjson/json_parser.cc \
    ../3rdparty/qjson/json_scanner.cpp \
    ../3rdparty/qtwebsocket/QtWebSocket/QWsServer.cpp \
    ../3rdparty/qtwebsocket/QtWebSocket/QWsSocket.cpp

HEADERS += \
    engine/area.h \
    engine/character.h \
    engine/characterstats.h \
    engine/class.h \
    engine/commandinterpreter.h \
    engine/constants.h \
    engine/engine.h \
    engine/exit.h \
    engine/gameexception.h \
    engine/gameobject.h \
    engine/gameobjectptr.h \
    engine/gameobjectsyncthread.h \
    engine/item.h \
    engine/player.h \
    engine/race.h \
    engine/realm.h \
    engine/scriptengine.h \
    engine/scriptfunction.h \
    engine/scriptfunctionmap.h \
    engine/scriptwindow.h \
    engine/util.h \
    engine/commands/closecommand.h \
    engine/commands/command.h \
    engine/commands/dropcommand.h \
    engine/commands/getcommand.h \
    engine/commands/givecommand.h \
    engine/commands/gocommand.h \
    engine/commands/inventorycommand.h \
    engine/commands/killcommand.h \
    engine/commands/lookcommand.h \
    engine/commands/opencommand.h \
    engine/commands/quitcommand.h \
    engine/commands/saycommand.h \
    engine/commands/shoutcommand.h \
    engine/commands/slashmecommand.h \
    engine/commands/statscommand.h \
    engine/commands/talkcommand.h \
    engine/commands/tellcommand.h \
    engine/commands/whocommand.h \
    engine/commands/admin/addcharactercommand.h \
    engine/commands/admin/addexitcommand.h \
    engine/commands/admin/additemcommand.h \
    engine/commands/admin/admincommand.h \
    engine/commands/admin/execscriptcommand.h \
    engine/commands/admin/getpropcommand.h \
    engine/commands/admin/gettriggercommand.h \
    engine/commands/admin/removeexitcommand.h \
    engine/commands/admin/removeitemcommand.h \
    engine/commands/admin/setpropcommand.h \
    engine/commands/admin/settriggercommand.h \
    engine/commands/admin/stopservercommand.h \
    engine/commands/admin/unsettriggercommand.h \
    interface/httpserver.h \
    interface/session.h \
    interface/telnetserver.h \
    interface/websocketserver.h \
    ../3rdparty/qjson/json_parser.hh \
    ../3rdparty/qjson/json_driver.hh \
    ../3rdparty/qjson/json_scanner.h \
    ../3rdparty/qtwebsocket/QtWebSocket/QWsServer.h \
    ../3rdparty/qtwebsocket/QtWebSocket/QWsSocket.h

OTHER_FILES += \
    ../web/admin.js \
    ../web/index.html \
    ../web/main.css \
    ../web/main.js \
    ../web/notifications.js \
    ../web/tappable.js

INCLUDEPATH += \
    $$PWD/../3rdparty \
    $$PWD/../3rdparty/qtwebsocket/QtWebSocket

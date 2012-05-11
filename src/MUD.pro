QT += core network
QT -= gui

TARGET = MUD
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
    main.cpp \
    engine/area.cpp \
    engine/badgameobjectexception.cpp \
    engine/character.cpp \
    engine/commandinterpreter.cpp \
    engine/engine.cpp \
    engine/exit.cpp \
    engine/gameobject.cpp \
    engine/gameobjectptr.cpp \
    engine/gameobjectsyncthread.cpp \
    engine/item.cpp \
    engine/realm.cpp \
    engine/util.cpp \
    engine/commands/command.cpp \
    engine/commands/dropcommand.cpp \
    engine/commands/getcommand.cpp \
    engine/commands/givecommand.cpp \
    engine/commands/gocommand.cpp \
    engine/commands/quitcommand.cpp \
    engine/commands/saycommand.cpp \
    engine/commands/slashmecommand.cpp \
    engine/commands/admin/addexitcommand.cpp \
    engine/commands/admin/additemcommand.cpp \
    engine/commands/admin/admincommand.cpp \
    engine/commands/admin/getpropcommand.cpp \
    engine/commands/admin/removeexitcommand.cpp \
    engine/commands/admin/removeitemcommand.cpp \
    engine/commands/admin/setpropcommand.cpp \
    engine/commands/admin/stopservercommand.cpp \
    interface/httpserver.cpp \
    interface/session.cpp \
    interface/websocketserver.cpp \
    ../3rdparty/qjson/json_driver.cc \
    ../3rdparty/qjson/json_parser.cc \
    ../3rdparty/qjson/json_scanner.cpp \
    ../3rdparty/qtwebsocket/QtWebSocket/QWsServer.cpp \
    ../3rdparty/qtwebsocket/QtWebSocket/QWsSocket.cpp

HEADERS += \
    engine/area.h \
    engine/badgameobjectexception.h \
    engine/character.h \
    engine/commandinterpreter.h \
    engine/engine.h \
    engine/exit.h \
    engine/gameobject.h \
    engine/gameobjectptr.h \
    engine/gameobjectsyncthread.h \
    engine/item.h \
    engine/realm.h \
    engine/util.h \
    engine/commands/command.h \
    engine/commands/dropcommand.h \
    engine/commands/getcommand.h \
    engine/commands/givecommand.h \
    engine/commands/gocommand.h \
    engine/commands/quitcommand.h \
    engine/commands/saycommand.h \
    engine/commands/slashmecommand.h \
    engine/commands/admin/addexitcommand.h \
    engine/commands/admin/additemcommand.h \
    engine/commands/admin/admincommand.h \
    engine/commands/admin/getpropcommand.h \
    engine/commands/admin/removeexitcommand.h \
    engine/commands/admin/removeitemcommand.h \
    engine/commands/admin/setpropcommand.h \
    engine/commands/admin/stopservercommand.h \
    interface/httpserver.h \
    interface/session.h \
    interface/websocketserver.h \
    ../3rdparty/qjson/json_parser.hh \
    ../3rdparty/qjson/json_driver.hh \
    ../3rdparty/qjson/json_scanner.h \
    ../3rdparty/qtwebsocket/QtWebSocket/QWsServer.h \
    ../3rdparty/qtwebsocket/QtWebSocket/QWsSocket.h

OTHER_FILES += \
    ../web/index.html \
    ../web/main.css \
    ../web/main.js

INCLUDEPATH += \
    $$PWD/../3rdparty \
    $$PWD/../3rdparty/qtwebsocket/QtWebSocket

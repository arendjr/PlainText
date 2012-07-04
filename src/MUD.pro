QT += core network script
QT -= gui

macx {
    QMAKE_CXXFLAGS = -std=c++11 -stdlib=libc++
}
!macx {
    QMAKE_CXXFLAGS = -std=c++0x
}

DEFINES *= QT_USE_QSTRINGBUILDER

TARGET = MUD
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
    main.cpp \
    engine/application.cpp \
    engine/area.cpp \
    engine/character.cpp \
    engine/characterstats.cpp \
    engine/class.cpp \
    engine/commandevent.cpp \
    engine/commandinterpreter.cpp \
    engine/deleteobjectevent.cpp \
    engine/effect.cpp \
    engine/engine.cpp \
    engine/event.cpp \
    engine/exit.cpp \
    engine/gameexception.cpp \
    engine/gameobject.cpp \
    engine/gameobjectptr.cpp \
    engine/gameobjectsyncthread.cpp \
    engine/gamethread.cpp \
    engine/item.cpp \
    engine/modifier.cpp \
    engine/player.cpp \
    engine/race.cpp \
    engine/realm.cpp \
    engine/scriptengine.cpp \
    engine/scriptfunction.cpp \
    engine/scriptfunctionmap.cpp \
    engine/scriptwindow.cpp \
    engine/session.cpp \
    engine/timerevent.cpp \
    engine/util.cpp \
    engine/commands/buycommand.cpp \
    engine/commands/closecommand.cpp \
    engine/commands/command.cpp \
    engine/commands/drinkcommand.cpp \
    engine/commands/dropcommand.cpp \
    engine/commands/eatcommand.cpp \
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
    engine/commands/usecommand.cpp \
    engine/commands/whocommand.cpp \
    engine/commands/admin/addcharactercommand.cpp \
    engine/commands/admin/addexitcommand.cpp \
    engine/commands/admin/additemcommand.cpp \
    engine/commands/admin/admincommand.cpp \
    engine/commands/admin/copyitemcommand.cpp \
    engine/commands/admin/execscriptcommand.cpp \
    engine/commands/admin/getpropcommand.cpp \
    engine/commands/admin/gettriggercommand.cpp \
    engine/commands/admin/listpropscommand.cpp \
    engine/commands/admin/removeexitcommand.cpp \
    engine/commands/admin/removeitemcommand.cpp \
    engine/commands/admin/setclasscommand.cpp \
    engine/commands/admin/setpropcommand.cpp \
    engine/commands/admin/setracecommand.cpp \
    engine/commands/admin/settriggercommand.cpp \
    engine/commands/admin/stopservercommand.cpp \
    engine/commands/admin/unsettriggercommand.cpp \
    interface/httpserver.cpp \
    interface/telnetserver.cpp \
    interface/websocketserver.cpp \
    ../3rdparty/qjson/json_driver.cc \
    ../3rdparty/qjson/json_parser.cc \
    ../3rdparty/qjson/json_scanner.cpp \
    ../3rdparty/qtwebsocket/QtWebSocket/QWsServer.cpp \
    ../3rdparty/qtwebsocket/QtWebSocket/QWsSocket.cpp

HEADERS += \
    engine/application.h \
    engine/area.h \
    engine/character.h \
    engine/characterstats.h \
    engine/class.h \
    engine/commandevent.h \
    engine/commandinterpreter.h \
    engine/constants.h \
    engine/deleteobjectevent.h \
    engine/effect.h \
    engine/engine.h \
    engine/event.h \
    engine/exit.h \
    engine/gameexception.h \
    engine/gameobject.h \
    engine/gameobjectptr.h \
    engine/gameobjectsyncthread.h \
    engine/gamethread.h \
    engine/item.h \
    engine/modifier.h \
    engine/player.h \
    engine/race.h \
    engine/realm.h \
    engine/scriptengine.h \
    engine/scriptfunction.h \
    engine/scriptfunctionmap.h \
    engine/scriptwindow.h \
    engine/session.h \
    engine/timerevent.h \
    engine/util.h \
    engine/commands/buycommand.h \
    engine/commands/closecommand.h \
    engine/commands/command.h \
    engine/commands/drinkcommand.h \
    engine/commands/dropcommand.h \
    engine/commands/eatcommand.h \
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
    engine/commands/usecommand.h \
    engine/commands/whocommand.h \
    engine/commands/admin/addcharactercommand.h \
    engine/commands/admin/addexitcommand.h \
    engine/commands/admin/additemcommand.h \
    engine/commands/admin/admincommand.h \
    engine/commands/admin/copyitemcommand.h \
    engine/commands/admin/execscriptcommand.h \
    engine/commands/admin/getpropcommand.h \
    engine/commands/admin/gettriggercommand.h \
    engine/commands/admin/listpropscommand.h \
    engine/commands/admin/removeexitcommand.h \
    engine/commands/admin/removeitemcommand.h \
    engine/commands/admin/setclasscommand.h \
    engine/commands/admin/setpropcommand.h \
    engine/commands/admin/setracecommand.h \
    engine/commands/admin/settriggercommand.h \
    engine/commands/admin/stopservercommand.h \
    engine/commands/admin/unsettriggercommand.h \
    interface/httpserver.h \
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
    ../web/tappable.js \
    engine/util.js \
    ../DESIGN.txt

INCLUDEPATH += \
    $$PWD/../3rdparty \
    $$PWD/../3rdparty/qtwebsocket/QtWebSocket

RESOURCES += \
    engine/resources.qrc

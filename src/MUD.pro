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
    engine/engine.cpp \
    engine/gameobject.cpp \
    engine/gameobjectptr.cpp \
    engine/realm.cpp \
    interface/httpserver.cpp \
    interface/websocketserver.cpp \
    ../3rdparty/qjson/json_driver.cc \
    ../3rdparty/qjson/json_parser.cc \
    ../3rdparty/qjson/json_scanner.cpp \
    interface/session.cpp \
    engine/exit.cpp \
    interface/commandinterpreter.cpp

HEADERS += \
    engine/area.h \
    engine/badgameobjectexception.h \
    engine/character.h \
    engine/engine.h \
    engine/gameobject.h \
    engine/gameobjectptr.h \
    engine/realm.h \
    interface/httpserver.h \
    interface/websocketserver.h \
    ../3rdparty/qjson/json_driver.hh \
    ../3rdparty/qjson/json_parser.hh \
    ../3rdparty/qjson/json_scanner.h \
    interface/session.h \
    engine/exit.h \
    interface/commandinterpreter.h

OTHER_FILES += \
    ../web/index.html \
    ../web/main.css \
    ../web/main.js

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../3rdparty/qtwebsocket/QtWebSocket/release/ -lQtWebSocket
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../3rdparty/qtwebsocket/QtWebSocket/debug/ -lQtWebSocket
else:unix:!symbian: LIBS += -L$$OUT_PWD/../3rdparty/qtwebsocket/QtWebSocket/ -lQtWebSocket

INCLUDEPATH += \
    $$PWD/../3rdparty \
    $$PWD/../3rdparty/qtwebsocket/QtWebSocket
DEPENDPATH += \
    $$PWD/../3rdparty/qtwebsocket/QtWebSocket

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../3rdparty/qtwebsocket/QtWebSocket/release/QtWebSocket.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../3rdparty/qtwebsocket/QtWebSocket/debug/QtWebSocket.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../3rdparty/qtwebsocket/QtWebSocket/libQtWebSocket.a

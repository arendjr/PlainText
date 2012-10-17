QT += core network script
QT -= gui

macx {
    DEPLOYMENT_TARGET = 10.7
    MAC_SDK  = /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX$${DEPLOYMENT_TARGET}.sdk
    if (!exists($$MAC_SDK)) {
        error("The selected Mac OS X SDK does not exist at $$MAC_SDK!")
    }
    QMAKE_MACOSX_DEPLOYMENT_TARGET = $$DEPLOYMENT_TARGET
    QMAKE_MAC_SDK = $$MAC_SDK
    QMAKE_CXXFLAGS = -stdlib=libc++
    LIBS += -L$$MAC_SDK/usr/lib/ -lc++
} else {
    system(which clang++ 2> /dev/null) {
        LIBS += -lc++
        QMAKE_CXX = clang++
        QMAKE_CXXFLAGS = -stdlib=libc++
    }
}
QMAKE_CXXFLAGS += -std=c++11
LIBS += -lz

DEFINES *= QT_USE_QSTRINGBUILDER

TARGET = PlainText
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
    main.cpp \
    engine/application.cpp \
    engine/character.cpp \
    engine/characterstats.cpp \
    engine/class.cpp \
    engine/commandinterpreter.cpp \
    engine/commandregistry.cpp \
    engine/container.cpp \
    engine/conversionutil.cpp \
    engine/diskutil.cpp \
    engine/effect.cpp \
    engine/engine.cpp \
    engine/exit.cpp \
    engine/gameexception.cpp \
    engine/gameobject.cpp \
    engine/gameobjectptr.cpp \
    engine/gameobjectsyncthread.cpp \
    engine/gamethread.cpp \
    engine/group.cpp \
    engine/item.cpp \
    engine/logthread.cpp \
    engine/logutil.cpp \
    engine/modifier.cpp \
    engine/player.cpp \
    engine/race.cpp \
    engine/realm.cpp \
    engine/room.cpp \
    engine/scriptengine.cpp \
    engine/scriptfunction.cpp \
    engine/scriptfunctionmap.cpp \
    engine/scriptwindow.cpp \
    engine/session.cpp \
    engine/shield.cpp \
    engine/statsitem.cpp \
    engine/util.cpp \
    engine/weapon.cpp \
    engine/commands/buycommand.cpp \
    engine/commands/closecommand.cpp \
    engine/commands/command.cpp \
    engine/commands/descriptioncommand.cpp \
    engine/commands/disbandcommand.cpp \
    engine/commands/dropcommand.cpp \
    engine/commands/eatcommand.cpp \
    engine/commands/equipmentcommand.cpp \
    engine/commands/followcommand.cpp \
    engine/commands/getcommand.cpp \
    engine/commands/givecommand.cpp \
    engine/commands/gocommand.cpp \
    engine/commands/groupcommand.cpp \
    engine/commands/gtalkcommand.cpp \
    engine/commands/helpcommand.cpp \
    engine/commands/inventorycommand.cpp \
    engine/commands/killcommand.cpp \
    engine/commands/lookcommand.cpp \
    engine/commands/losecommand.cpp \
    engine/commands/opencommand.cpp \
    engine/commands/quitcommand.cpp \
    engine/commands/removecommand.cpp \
    engine/commands/saycommand.cpp \
    engine/commands/scriptcommand.cpp \
    engine/commands/searchcommand.cpp \
    engine/commands/shoutcommand.cpp \
    engine/commands/slashmecommand.cpp \
    engine/commands/statscommand.cpp \
    engine/commands/talkcommand.cpp \
    engine/commands/tellcommand.cpp \
    engine/commands/usecommand.cpp \
    engine/commands/whocommand.cpp \
    engine/commands/wieldcommand.cpp \
    engine/commands/admin/addcharactercommand.cpp \
    engine/commands/admin/addcontainercommand.cpp \
    engine/commands/admin/addexitcommand.cpp \
    engine/commands/admin/additemcommand.cpp \
    engine/commands/admin/addshieldcommand.cpp \
    engine/commands/admin/addweaponcommand.cpp \
    engine/commands/admin/admincommand.cpp \
    engine/commands/admin/copyitemcommand.cpp \
    engine/commands/admin/copytriggerscommand.cpp \
    engine/commands/admin/execscriptcommand.cpp \
    engine/commands/admin/getpropcommand.cpp \
    engine/commands/admin/gettriggercommand.cpp \
    engine/commands/admin/listmethodscommand.cpp \
    engine/commands/admin/listpropscommand.cpp \
    engine/commands/admin/removeexitcommand.cpp \
    engine/commands/admin/removeitemcommand.cpp \
    engine/commands/admin/setclasscommand.cpp \
    engine/commands/admin/setpropcommand.cpp \
    engine/commands/admin/setracecommand.cpp \
    engine/commands/admin/settriggercommand.cpp \
    engine/commands/admin/stopservercommand.cpp \
    engine/commands/admin/unsettriggercommand.cpp \
    engine/commands/api/apicommand.cpp \
    engine/commands/api/datagetcommand.cpp \
    engine/commands/api/datasetcommand.cpp \
    engine/commands/api/exitsetcommand.cpp \
    engine/commands/api/exitslistcommand.cpp \
    engine/commands/api/logretrievecommand.cpp \
    engine/commands/api/propertygetcommand.cpp \
    engine/commands/api/propertysetcommand.cpp \
    engine/commands/api/roomslistcommand.cpp \
    engine/commands/api/triggergetcommand.cpp \
    engine/commands/api/triggersetcommand.cpp \
    engine/commands/api/triggerslistcommand.cpp \
    engine/events/asyncreplyevent.cpp \
    engine/events/commandevent.cpp \
    engine/events/deleteobjectevent.cpp \
    engine/events/event.cpp \
    engine/events/signinevent.cpp \
    engine/events/timerevent.cpp \
    engine/logmessages/commandlogmessage.cpp \
    engine/logmessages/logmessage.cpp \
    engine/logmessages/npctalklogmessage.cpp \
    engine/logmessages/playerdeathstatslogmessage.cpp \
    engine/logmessages/retrievestatslogmessage.cpp \
    engine/logmessages/roomvisitstatslogmessage.cpp \
    engine/logmessages/sessionlogmessage.cpp \
    interface/httpserver.cpp \
    interface/telnetserver.cpp \
    interface/websocketserver.cpp \
    ../3rdparty/qjson/json_driver.cpp \
    ../3rdparty/qjson/json_parser.cpp \
    ../3rdparty/qjson/json_scanner.cpp \
    ../3rdparty/qtiocompressor/qtiocompressor.cpp \
    ../3rdparty/qtwebsocket/QtWebSocket/QWsServer.cpp \
    ../3rdparty/qtwebsocket/QtWebSocket/QWsSocket.cpp

HEADERS += \
    engine/application.h \
    engine/character.h \
    engine/characterstats.h \
    engine/class.h \
    engine/commandinterpreter.h \
    engine/commandregistry.h \
    engine/constants.h \
    engine/container.h \
    engine/conversionutil.h \
    engine/diskutil.h \
    engine/effect.h \
    engine/engine.h \
    engine/exit.h \
    engine/gameexception.h \
    engine/gameobject.h \
    engine/gameobjectptr.h \
    engine/gameobjectsyncthread.h \
    engine/gamethread.h \
    engine/group.h \
    engine/item.h \
    engine/logthread.h \
    engine/logutil.h \
    engine/modifier.h \
    engine/player.h \
    engine/race.h \
    engine/realm.h \
    engine/room.h \
    engine/scriptengine.h \
    engine/scriptfunction.h \
    engine/scriptfunctionmap.h \
    engine/scriptwindow.h \
    engine/session.h \
    engine/shield.h \
    engine/statsitem.h \
    engine/util.h \
    engine/weapon.h \
    engine/commands/buycommand.h \
    engine/commands/closecommand.h \
    engine/commands/command.h \
    engine/commands/descriptioncommand.h \
    engine/commands/disbandcommand.h \
    engine/commands/dropcommand.h \
    engine/commands/eatcommand.h \
    engine/commands/equipmentcommand.h \
    engine/commands/followcommand.h \
    engine/commands/getcommand.h \
    engine/commands/givecommand.h \
    engine/commands/gocommand.h \
    engine/commands/groupcommand.h \
    engine/commands/gtalkcommand.h \
    engine/commands/helpcommand.h \
    engine/commands/inventorycommand.h \
    engine/commands/killcommand.h \
    engine/commands/lookcommand.h \
    engine/commands/losecommand.h \
    engine/commands/opencommand.h \
    engine/commands/quitcommand.h \
    engine/commands/removecommand.h \
    engine/commands/saycommand.h \
    engine/commands/scriptcommand.h \
    engine/commands/searchcommand.h \
    engine/commands/shoutcommand.h \
    engine/commands/slashmecommand.h \
    engine/commands/statscommand.h \
    engine/commands/talkcommand.h \
    engine/commands/tellcommand.h \
    engine/commands/usecommand.h \
    engine/commands/whocommand.h \
    engine/commands/wieldcommand.h \
    engine/commands/admin/addcharactercommand.h \
    engine/commands/admin/addcontainercommand.h \
    engine/commands/admin/addexitcommand.h \
    engine/commands/admin/additemcommand.h \
    engine/commands/admin/addshieldcommand.h \
    engine/commands/admin/addweaponcommand.h \
    engine/commands/admin/admincommand.h \
    engine/commands/admin/copyitemcommand.h \
    engine/commands/admin/copytriggerscommand.h \
    engine/commands/admin/execscriptcommand.h \
    engine/commands/admin/getpropcommand.h \
    engine/commands/admin/gettriggercommand.h \
    engine/commands/admin/listmethodscommand.h \
    engine/commands/admin/listpropscommand.h \
    engine/commands/admin/removeexitcommand.h \
    engine/commands/admin/removeitemcommand.h \
    engine/commands/admin/setclasscommand.h \
    engine/commands/admin/setpropcommand.h \
    engine/commands/admin/setracecommand.h \
    engine/commands/admin/settriggercommand.h \
    engine/commands/admin/stopservercommand.h \
    engine/commands/admin/unsettriggercommand.h \
    engine/commands/api/apicommand.h \
    engine/commands/api/datagetcommand.h \
    engine/commands/api/datasetcommand.h \
    engine/commands/api/exitsetcommand.h \
    engine/commands/api/exitslistcommand.h \
    engine/commands/api/logretrievecommand.h \
    engine/commands/api/propertygetcommand.h \
    engine/commands/api/propertysetcommand.h \
    engine/commands/api/roomslistcommand.h \
    engine/commands/api/triggergetcommand.h \
    engine/commands/api/triggersetcommand.h \
    engine/commands/api/triggerslistcommand.h \
    engine/events/asyncreplyevent.h \
    engine/events/commandevent.h \
    engine/events/deleteobjectevent.h \
    engine/events/event.h \
    engine/events/signinevent.h \
    engine/events/timerevent.h \
    engine/logmessages/commandlogmessage.h \
    engine/logmessages/logmessage.h \
    engine/logmessages/npctalklogmessage.h \
    engine/logmessages/playerdeathstatslogmessage.h \
    engine/logmessages/retrievestatslogmessage.h \
    engine/logmessages/roomvisitstatslogmessage.h \
    engine/logmessages/sessionlogmessage.h \
    interface/httpserver.h \
    interface/telnetserver.h \
    interface/websocketserver.h \
    ../3rdparty/qjson/json_parser.hh \
    ../3rdparty/qjson/json_driver.hh \
    ../3rdparty/qjson/json_scanner.h \
    ../3rdparty/qtiocompressor/qtiocompressor.h \
    ../3rdparty/qtwebsocket/QtWebSocket/QWsServer.h \
    ../3rdparty/qtwebsocket/QtWebSocket/QWsSocket.h

OTHER_FILES += \
    engine/util.js \
    engine/commands/command.js \
    ../data/commands/drinkcommand.js \
    ../data/commands/putcommand.js \
    ../web/index.html \
    ../web/kinetic.js \
    ../web/main.css \
    ../web/main.js \
    ../web/notifications.js \
    ../web/admin/admin.css \
    ../web/admin/admin.js \
    ../web/admin/exiteditor.js \
    ../web/admin/mapeditor.js \
    ../web/admin/map.model.js \
    ../web/admin/map.view.js \
    ../web/admin/propertyeditor.js \
    ../web/admin/slider.widget.js \
    ../DESIGN.txt \
    ../README.md \
    ../TODO.txt

INCLUDEPATH += \
    engine \
    engine/commands \
    engine/commands/admin \
    engine/commands/api \
    engine/events \
    engine/logmessages \
    $$PWD/../3rdparty \
    $$PWD/../3rdparty/qtiocompressor \
    $$PWD/../3rdparty/qtwebsocket/QtWebSocket

RESOURCES += \
    engine/resources.qrc

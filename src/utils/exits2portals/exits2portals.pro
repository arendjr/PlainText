include(../../environment.pri)

TARGET = PlainText

TEMPLATE = app

SOURCES += \
    conversionutil.cpp \
    diskutil.cpp \
    exit.cpp \
    gameexception.cpp \
    gameobject.cpp \
    gameobjectptr.cpp \
    main.cpp \
    metatyperegistry.cpp \
    point3d.cpp \
    portal.cpp \
    realm.cpp \
    room.cpp \
    scriptfunction.cpp \
    scriptfunctionmap.cpp \
    ../../../3rdparty/qjson/json_driver.cpp \
    ../../../3rdparty/qjson/json_parser.cpp \
    ../../../3rdparty/qjson/json_scanner.cpp \

HEADERS += \
    constants.h \
    conversionutil.h \
    diskutil.h \
    exit.h \
    foreach.h \
    gameexception.h \
    gameobject.h \
    gameobjectptr.h \
    metatyperegistry.h \
    point3d.h \
    portal.h \
    realm.h \
    room.h \
    scriptfunction.h \
    scriptfunctionmap.h \
    ../../../3rdparty/qjson/json_parser.hh \
    ../../../3rdparty/qjson/json_driver.hh \
    ../../../3rdparty/qjson/json_scanner.h \

INCLUDEPATH += \
    $$PWD/../../../3rdparty \

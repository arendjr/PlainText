include(../../environment.pri)

TARGET = modify-portal-flags

TEMPLATE = app

SOURCES += \
    conversionutil.cpp \
    diskutil.cpp \
    gameeventmultipliermap.cpp \
    gameexception.cpp \
    gameobject.cpp \
    gameobjectptr.cpp \
    main.cpp \
    metatyperegistry.cpp \
    portal.cpp \
    realm.cpp \
    scriptfunction.cpp \
    scriptfunctionmap.cpp \
    ../../../3rdparty/qjson/json_driver.cpp \
    ../../../3rdparty/qjson/json_parser.cpp \
    ../../../3rdparty/qjson/json_scanner.cpp \

HEADERS += \
    constants.h \
    conversionutil.h \
    diskutil.h \
    foreach.h \
    gameeventmultipliermap.h \
    gameexception.h \
    gameobject.h \
    gameobjectptr.h \
    metatyperegistry.h \
    portal.h \
    realm.h \
    scriptfunction.h \
    scriptfunctionmap.h \
    ../../../3rdparty/qjson/json_parser.hh \
    ../../../3rdparty/qjson/json_driver.hh \
    ../../../3rdparty/qjson/json_scanner.h \

INCLUDEPATH += \
    $$PWD/../../../3rdparty \

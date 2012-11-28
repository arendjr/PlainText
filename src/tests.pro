include(PlainText.pro)

TARGET = tests

CONFIG += qtestlib

SOURCES -= \
    main.cpp \

SOURCES += \
    tests/main.cpp \
    tests/testcase.cpp \

HEADERS += \
    tests/testcase.h \
    tests/test_container.h \
    tests/test_crashes.h \
    tests/test_serialization.h \
    tests/test_visualevents.h \

INCLUDEPATH += \
    tests \

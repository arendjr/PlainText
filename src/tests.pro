include(PlainText.pro)

TARGET = tests

CONFIG  += qtestlib

SOURCES -= \
    main.cpp \

SOURCES += \
    tests/main.cpp \
    tests/testcase.cpp \

HEADERS += \
    tests/testcase.h \
    tests/test_serialization.h \

INCLUDEPATH += \
    tests \

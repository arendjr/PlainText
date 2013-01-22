include(PlainText.pro)

TARGET = tests

CONFIG += qtestlib

SOURCES -= \
    src/main.cpp \

SOURCES += \
    src/tests/main.cpp \
    src/tests/testcase.cpp \

HEADERS += \
    src/tests/testcase.h \
    src/tests/test_container.h \
    src/tests/test_crashes.h \
    src/tests/test_floodevent.h \
    src/tests/test_help.h \
    src/tests/test_movement.h \
    src/tests/test_openandclose.h \
    src/tests/test_serialization.h \
    src/tests/test_visualevents.h \

INCLUDEPATH += \
    src/tests \

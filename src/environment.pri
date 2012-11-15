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

DEFINES *= QT_USE_QSTRINGBUILDER

CONFIG += console debug
CONFIG -= app_bundle

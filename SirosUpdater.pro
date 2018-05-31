TEMPLATE = app

QT += qml quick widgets serialport
QT += core

TARGET = SirosUpdater

win32 {
    LIBS += Psapi.lib
}

SOURCES += main.cpp \
    bsl/BSL_Interface.cpp \
    bsl/BSL_IO_UART.cpp \
    bsl/TextFileIO.cpp \
    bsl/CRC.cpp \
    updatedriver.cpp \
    updateworker.cpp \
    firmwareinfo.cpp \
    targetinfo.cpp

RESOURCES += SirosUpdater.qrc

RC_FILE = SirosUpdater.rc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(SirosUpdater.pri)

HEADERS += \
    bsl/BSL_Common.h \
    bsl/BSL_Interface.h \
    bsl/BSL_IO_UART.h \
    bsl/TextFileIO.h \
    bsl/CRC.h \
    updatedriver.h \
    updateworker.h \
    firmwareinfo.h \
    targetinfo.h

OTHER_FILES +=

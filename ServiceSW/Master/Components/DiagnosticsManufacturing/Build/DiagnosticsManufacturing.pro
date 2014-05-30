#-------------------------------------------------
#
# Project created by QtCreator 2013-04-23T13:48:34
#
#-------------------------------------------------

!include(DiagnosticsManufacturing.pri) {
    error("DiagnosticsManufacturing.pri not found")
}

TARGET = DiagnosticsManufacturing

HEADERS *= ../Include/*.h \          # header file location
    ../Include/MainControlManufacturing.h

SOURCES *= ../Source/*.cpp \         # source file location
    ../Source/MainControlManufacturing.cpp \
    ../Source/LaSystemManufacturing.cpp

FORMS += ../Forms/*.ui \
    ../Forms/MainControlManufacturing.ui \
    ../Forms/LaSystemManufacturing.ui

RESOURCES *= ../Resource/*.qrc   # resource file location



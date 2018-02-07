!include("TestSettings.pri"):error("TestSettings.pri not found")

TARGET = utTestSettings
CONFIG   += console
SOURCES += TestSettings.cpp

TEMPLATE = app


INCLUDEPATH += ../../ \
               ../../../../../Shared/Master/Components \
               ../../../../../../Platform/Master/Components \
               ../../../../../../Platform/Gui/Components \
               ../../../../../HimalayaMain/Master/Include

CONFIG += qtestlib
CONFIG += qt
DEPENDPATH += ../../../../

UseLibs(Settings Dashboard Reagents Core NetworkLayer)
UsePlatformGUILibs(MainMenu KeyBoard Application)
UsePlatformLibs(DataManager Global NetCommands NetworkComponents)
UseSharedLibs(HimalayaDataContainer)
UsePlatformScroller(QtScroller)




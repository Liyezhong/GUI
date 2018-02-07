!include("TestReagents.pri"):error("TestReagents.pri not found")

TARGET = utTestReagents
CONFIG   += console
SOURCES += TestReagents.cpp \

TEMPLATE = app


INCLUDEPATH += ../../ \
               ../../../../../Shared/Master/Components \
               ../../../../../../Platform/Master/Components \
               ../../../../../../Platform/Gui/Components \
               ../../../../../HimalayaMain/Master/Include

CONFIG += qtestlib
CONFIG += qt
DEPENDPATH += ../../../../

UseLibs(Reagents Core NetworkLayer)
UsePlatformGUILibs(MainMenu KeyBoard Application)
UsePlatformLibs(DataManager Global NetCommands NetworkComponents)
UseSharedLibs(HimalayaDataContainer)
UsePlatformScroller(QtScroller)


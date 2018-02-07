!include("TestDashboard.pri"):error("TestDashboard.pri not found")

TARGET = utTestDashboard
CONFIG   += console
SOURCES += TestDashboard.cpp
TEMPLATE = app


INCLUDEPATH += ../../ \
               ../../../../../Shared/Master/Components \
               ../../../../../../Platform/Master/Components \
               ../../../../../../Platform/Gui/Components \
               ../../../../../HimalayaMain/Master/Include

CONFIG += qtestlib
CONFIG += qt
DEPENDPATH += ../../../../

################# start group
LIBS += -Wl,--start-group \

UseLibs(Dashboard Core NetworkLayer)
UsePlatformGUILibs(MainMenu KeyBoard Application)
UsePlatformLibs(DataManager Global NetCommands NetworkComponents)
UseSharedLibs(HimalayaDataContainer)
UsePlatformScroller(QtScroller)

################# end group
LIBS += -Wl,--end-group
#HEADERS   *= ../Include/*.h      # header file location

HEADERS +=
LIBS += -ldl

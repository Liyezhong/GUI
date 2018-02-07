!include("StubHimalayaGuiController.pri") {
    error("StubHimalayaGuiController.pri not found")
}

TARGET = StubHimalayaGuiController

HEADERS +=  ../Include/*.h

SOURCES +=  ../Source/*.cpp

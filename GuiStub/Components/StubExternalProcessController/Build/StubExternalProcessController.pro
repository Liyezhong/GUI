!include("StubExternalProcessController.pri") {
    error("StubExternalProcessController.pri not found")
}

TARGET = StubExternalProcessController

HEADERS +=  ../Include/*.h \
            ../Include/ExternalProcessStates/*.h

SOURCES +=  ../Source/*.cpp \
            ../Source/ExternalProcessStates/*.cpp


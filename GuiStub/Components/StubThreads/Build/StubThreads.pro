!include("StubThreads.pri") {
    error("StubThreads.pri not found")
}



TARGET = StubThreads
INCLUDEPATH += ../../../../Components \
               ../../../../../../../Platform/Master/Components/ 

HEADERS +=  ../Include/*.h

SOURCES +=  ../Source/*.cpp

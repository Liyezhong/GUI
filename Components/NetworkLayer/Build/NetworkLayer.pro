!include(NetworkLayer.pri) {
    error("NetworkLayer.pri not found")
}

TARGET   = NetworkLayer         # functional unit name
HEADERS += ../Include/*.h
SOURCES += ../Source/*.cpp

!include(Core.pri):error("Core.pri not found")
TARGET = Core # functional unit name
HEADERS *= ../Include/*.h # header file location
SOURCES *= ../Source/*.cpp # source file location

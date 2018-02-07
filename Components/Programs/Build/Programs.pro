!include(Programs.pri):error("Programs.pri not found")
TARGET = Programs # functional unit name
HEADERS *= ../Include/*.h  # header file location
SOURCES *= ../Source/*.cpp  # source file location
FORMS *= ../Forms/*.ui # form file location

!include(Reagents.pri) {
    error("Reagents.pri not found")
}

TARGET = Reagents           # functional unit name
HEADERS *= ../Include/*.h   # header file location
SOURCES *= ../Source/*.cpp  # source file location
FORMS   *= ../Forms/*.ui    # form file location

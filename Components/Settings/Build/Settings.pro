!include(Settings.pri) {
    error("Settings.pri not found")
}

TARGET = Settings           # functional unit name
HEADERS *= ../Include/*.h   # header file location
SOURCES *= ../Source/*.cpp  # source file location
FORMS   *= ../Forms/*.ui    # form file location

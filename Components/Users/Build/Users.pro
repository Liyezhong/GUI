!include(Users.pri) {
    error("Users.pri not found")
}

TARGET = Users              # functional unit name
HEADERS *= ../Include/*.h   # header file location
SOURCES *= ../Source/*.cpp  # source file location
FORMS   *= ../Forms/*.ui    # form file location

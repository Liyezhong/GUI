!include(ServiceWidget.pri) {
    error("ServiceWidget.pri not found")
}

TARGET = ServiceWidget             # functional unit name

HEADERS *= ../Include/*.h         # header file location

SOURCES *= ../Source/*.cpp        # source file location

FORMS   *= ../Forms/*.ui          # form file location

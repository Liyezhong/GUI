!include(Dashboard.pri):error("Dashboard.pri not found")
TARGET = Dashboard # functional unit name
HEADERS += ../Include/*.h      # header file location
SOURCES += ../Source/*.cpp     # source file location
FORMS   += ../Forms/*.ui      # form file location

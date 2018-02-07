!include("TestHimalayaMasterThread.pri"):error("TestHimalayaMasterThread.pri not found")
TARGET = TestHimalayaMasterThread

HEADERS +=  ../Include/*.h \
            ../../HimalayaDataManager/Include/*.h \
            ../../HimalayaDataManager/CommandInterface/Include/*.h

SOURCES +=  ../Source/*.cpp \
            ../../HimalayaDataManager/Source/*.cpp \
            ../../HimalayaDataManager/CommandInterface/Source/*.cpp

QT += gui

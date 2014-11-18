!include("../../../Test/Himalaya.pri") {
    error("../../../Test/Himalaya.pri not found")
}

TARGET = utTestHimalayaMasterThread
SOURCES += TestHimalayaMasterThread.cpp

INCLUDEPATH += ../../../../ \
  ../../../../../../../../Platform/Master/Components/ \
  ../../../../../Shared/Master/Components

DEPENDPATH += ../../../../


UseLibs(HimalayaDataManager HimalayaMasterThread Scheduler HimalayaGuiController)
UseLibsShared(HimalayaDataContainer)
UseLibsPlatform(ExternalProcessController Threads HeartBeatManager ExportController SWUpdateManager EncryptionDecryption RemoteCareController RemoteCareManager DataLogging Global StateMachines DeviceControl EventHandler DataManager NetCommands NetworkComponents PasswordManager)

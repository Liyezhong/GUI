# include pri file from HimalayaGui/Build

!include("../../../Build/HimalayaGui.pri") {
    error("../../../Build/HimalayaGui.pri not found")
}

#INCLUDEPATH += ../../../../../Platform/Master/Components/

#DEPENDPATH += ../../../../../Platform/Master/Components/

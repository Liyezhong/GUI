# include pri file from Test/Build

!include("../../../Build/GuiStub.pri") {
    error("../../../Build/GuiStub.pri not found")
}

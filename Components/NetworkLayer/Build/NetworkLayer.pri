# include pri file from SkylineGui/Build

!include("../../../Build/SkylineGui.pri") {
    error("../../../Build/SkylineGui.pri not found")
}

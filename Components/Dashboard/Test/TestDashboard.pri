# include pri file from Master/Test

!include("../../../Test/HimalayaGui.pri") {
    error("../../../Test/HimalayaGui.pri not found")
}

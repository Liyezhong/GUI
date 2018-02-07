# ################ include pri file #################
!include("Main.pri"):error("Main.pri not found")

# ###################################################
# ################ override some pri settings #################
TEMPLATE = app
TARGET = himalaya_gui_stub
CONFIG -= static
QT += gui \
    core \
    xml \
    network \
    xmlpatterns

#UseLib(../../../../../../../Platform/Master/Components/DeviceControl)
# #############################################################
# ################ list used platform libraries #################
#PLATFORM_GUI_COMPONENTS_DIR = ../../../../../../../Platform/GUI/Components
#PLATFORM_GUI_COMPONENTS = MainMenu

PLATFORM_COMPONENTS_DIR = ../../../../../../../Platform/Master/Components
PLATFORM_COMPONENTS = TestStubDataManager \
    #DataLogging \
    NetworkComponents \
    NetCommands \
    Global \
    #AxedaController \
    StateMachines \
    ExternalProcessController



# ###############################################################
# ################ list himalaya shared libraries #################
HIMALAYA_SHAREDCOMPONENTS_DIR = ../../../../../../Shared/Master/Components
HIMALAYA_SHAREDCOMPONENTS = HimalayaDataContainer \
#                            RMSCommands

# ##########################################################
# ################ list himalaya libraries #################
HIMALAYA_COMPONENTS_DIR = ../../../../../Master/Components
HIMALAYA_COMPONENTS = HimalayaGuiController \
     #                 HimalayaSepiaController \
     #                 Scheduler \
     #                 Verifier

# ##########################################################
# ################ list Test SW libraries #################
TEST_COMPONENTS_DIR = ../..
TEST_COMPONENTS = TestHimalayaMasterThread \
    Threads \
    CommandExecuter


# ##########################################################
# ################ own sources #################
INCLUDEPATH += ../..    \
            ../../../../../Master/Include \
            ../../../../ \
            ../../../../../../../Platform/Common/Components/ \
            #../../../../../../../Platform/Master/Components/DeviceCommandProcessor/Include/

HEADERS += ../Include/Main.h \
    ../Include/Startup.h \


SOURCES += ../Source/Main.cpp \
    ../Source/Startup.cpp \


#RESOURCES *= ../../../../../../Shared/Master/Components/DataManager/Resource/*.qrc # resource file location

# ##############################################
# ##########################################################################################
# ################ DO NOT TOUCH THIS UNLESS YOU KNOW WHAT YOU ARE DOING!!! #################
# ##########################################################################################
# ################ set destination path
DESTDIR = bin_$$CONFIG_SUFFIX

# ################ edit include path
INCLUDEPATH += $$PLATFORM_COMPONENTS_DIR \
    #$$PLATFORM_GUI_COMPONENTS_DIR \
    $$HIMALAYA_SHAREDCOMPONENTS_DIR \
    $$HIMALAYA_COMPONENTS_DIR \
    $$TEST_COMPONENTS_DIR

# ################ start group
LIBS += -Wl,--start-group

# ################ include platform libraries and set dependencies
for(TheComponent, PLATFORM_COMPONENTS) { 
    THELIBPATH = $$PLATFORM_COMPONENTS_DIR/$${TheComponent}/Build/lib_$$CONFIG_SUFFIX
    PRE_TARGETDEPS += $$THELIBPATH/lib$${TheComponent}.a
    LIBS += $$THELIBPATH/lib$${TheComponent}.a
}

# ################ include platform libraries and set dependencies

#for(TheComponent, PLATFORM_GUI_COMPONENTS) {
#    THELIBPATH = $$PLATFORM_GUI_COMPONENTS_DIR/$${TheComponent}/Build/lib_$$CONFIG_SUFFIX
#    PRE_TARGETDEPS += $$THELIBPATH/lib$${TheComponent}.a
#    LIBS += $$THELIBPATH/lib$${TheComponent}.a
#}


# ################ include himalaya shared libraries and set dependencies
for(TheComponent, HIMALAYA_SHAREDCOMPONENTS) { 
    THELIBPATH = $$HIMALAYA_SHAREDCOMPONENTS_DIR/$${TheComponent}/Build/lib_$$CONFIG_SUFFIX
    PRE_TARGETDEPS += $$THELIBPATH/lib$${TheComponent}.a
    LIBS += $$THELIBPATH/lib$${TheComponent}.a
}

# ################ include himalaya libraries and set dependencies
for(TheComponent, HIMALAYA_COMPONENTS) { 
    THELIBPATH = $$HIMALAYA_COMPONENTS_DIR/$${TheComponent}/Build/lib_$$CONFIG_SUFFIX
    PRE_TARGETDEPS += $$THELIBPATH/lib$${TheComponent}.a
    LIBS += $$THELIBPATH/lib$${TheComponent}.a
}

# ################ include Test SW libraries and set dependencies
for(TheComponent, TEST_COMPONENTS) { 
    THELIBPATH = $$TEST_COMPONENTS_DIR/$${TheComponent}/Build/lib_$$CONFIG_SUFFIX
    PRE_TARGETDEPS += $$THELIBPATH/lib$${TheComponent}.a
    LIBS += $$THELIBPATH/lib$${TheComponent}.a
}
#LIBS += ../../../../../../../Platform/Master/Components/DeviceControl/Build/lib_$$CONFIG_SUFFIX/libDeviceControl.a
#LIBS +=../../../../../../../Platform/Master/Components/ImportExport/Build/lib_$$CONFIG_SUFFIX/libImportExport.a
#LIBS +=../../../../../../../Platform/Master/Components/SoftSwitchManager/Build/lib_$$CONFIG_SUFFIX/libSoftSwitchManager.a
# ################ end group
LIBS += -Wl,--end-group
FORMS *= ../Forms/*.ui
#         ../../../../../Master/Components/Scheduler/Demo/SchedulerSimulation/*.ui

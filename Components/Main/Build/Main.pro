################# include pri file #################
!include("Main.pri") {
    error("Main.pri not found")
}
####################################################

################# override some pri settings #################
TEMPLATE = app
TARGET = himalaya_gui
CONFIG -= static
QT +=   xml \
        network \
        xmlpatterns
##############################################################


################# list used platform libraries #################
PLATFORM_COMPONENTS_DIR = ../../../../../../Platform/Master/Components
PLATFORM_COMPONENTS = NetworkComponents \
                      NetCommands \
                      Global \
                      DataManager \
#                      EventHandler \
#                      DataLogging
                      

################# list used platform libraries #################
PLATFORM_GUI_COMPONENTS_DIR = ../../../../../../Platform/Gui/Components
PLATFORM_GUI_COMPONENTS = Application \
                          KeyBoard \
                          MainMenu

################################################################

##############################################################

################# list used shared master libraries #################
SHARED_MASTER_COMPONENTS_DIR = ../../../../../Shared/Master/Components
SHARED_MASTER_COMPONENTS = HimalayaDataContainer
#                           NetCommands \
#                           RMSCommands

#####################################################################

################# list himalaya gui libraries #############
HIMALAYA_COMPONENTS_DIR = ../..
HIMALAYA_COMPONENTS = NetworkLayer \
                      Core \
                      Dashboard \
                      Programs \
                      Reagents \
                      Settings \
                      Users
###########################################################


################# own sources #################
INCLUDEPATH += ../..
SOURCES += ../Source/Main.cpp
###############################################





###########################################################################################
################# DO NOT TOUCH THIS UNLESS YOU KNOW WHAT YOU ARE DOING!!! #################
###########################################################################################

################# set destination path
DESTDIR = bin_$$CONFIG_SUFFIX

################# edit include path
INCLUDEPATH += $$PLATFORM_COMPONENTS_DIR \
               $$PLATFORM_GUI_COMPONENTS_DIR\
               $$SHARED_MASTER_COMPONENTS_DIR \
               $$HIMALAYA_COMPONENTS_DIR
               


################# start group
LIBS += -Wl,--start-group \

################# include kineticscroller library and set dependencies
THELIBPATH       = $$PLATFORM_GUI_COMPONENTS_DIR/kineticscroller/lib
PRE_TARGETDEPS  += $$THELIBPATH/libQtScroller.a
LIBS            += $$THELIBPATH/libQtScroller.a

################# include platform libraries and set dependencies
for(TheComponent, PLATFORM_COMPONENTS) {
    THELIBPATH       = $$PLATFORM_COMPONENTS_DIR/$${TheComponent}/Build/lib_$$CONFIG_SUFFIX
    PRE_TARGETDEPS  += $$THELIBPATH/lib$${TheComponent}.a
    LIBS            += $$THELIBPATH/lib$${TheComponent}.a
}

################# include platform libraries and set dependencies
for(TheComponent, PLATFORM_GUI_COMPONENTS) {
    THELIBPATH       = $$PLATFORM_GUI_COMPONENTS_DIR/$${TheComponent}/Build/lib_$$CONFIG_SUFFIX
    PRE_TARGETDEPS  += $$THELIBPATH/lib$${TheComponent}.a
    LIBS            += $$THELIBPATH/lib$${TheComponent}.a
}

################# include shared master libraries and set dependencies
for(TheComponent, SHARED_MASTER_COMPONENTS) {
    THELIBPATH       = $$SHARED_MASTER_COMPONENTS_DIR/$${TheComponent}/Build/lib_$$CONFIG_SUFFIX
    PRE_TARGETDEPS  += $$THELIBPATH/lib$${TheComponent}.a
    LIBS            += $$THELIBPATH/lib$${TheComponent}.a
}

################# include himalaya libraries and set dependencies
for(TheComponent, HIMALAYA_COMPONENTS) {
    THELIBPATH       = $$HIMALAYA_COMPONENTS_DIR/$${TheComponent}/Build/lib_$$CONFIG_SUFFIX
    PRE_TARGETDEPS  += $$THELIBPATH/lib$${TheComponent}.a
    LIBS            += $$THELIBPATH/lib$${TheComponent}.a
}

################# end group
LIBS += -Wl,--end-group
#HEADERS   *= ../Include/*.h      # header file location

HEADERS +=
LIBS += -ldl

######################
# common stuff for all components
######################


CONFIG += static \
          warn_on \
          qt \
          thread \
          rtti

QT += gui \
      core \
      xml \
      xmlpatterns \
      network

INCLUDEPATH += ../.. \
               ../../../../../../Platform/Master/Components \
               ../../../../../../Platform/Gui/Components \
               ../../../../../Shared/Gui/Components \
               ../../../../../Shared/Master/Components \
               ../../../../../HimalayaMain/Master/Include
               
DEPENDPATH +=  ../.. \
               ../../../../../../Platform/Master/Components \
               ../../../../../../Platform/Gui/Components \
               ../../../../../Shared/Gui/Components \
               ../../../../../Shared/Master/Components


CONFIG += debug_and_release
#CONFIG += build_all

# compute CONFIG_SUFFIX depending on debug / release
CONFIG(debug, debug|release) {
    CONFIG_SUFFIX = dbg
} else {
    CONFIG_SUFFIX = rel
    CONFIG(gcov) {
        QMAKE_CXXFLAGS_RELEASE += -fprofile-arcs -ftest-coverage
        QMAKE_LFLAGS_RELEASE += -fprofile-arcs
        QMAKE_CXXFLAGS_RELEASE -= -O2
        QMAKE_CXXFLAGS_RELEASE += -O0
    }
}

# set compile output directories
OBJECTS_DIR = obj_$$CONFIG_SUFFIX
MOC_DIR = moc_$$CONFIG_SUFFIX
DESTDIR = bin_$$CONFIG_SUFFIX


###################################
### stuff for adding libraries ####
###################################
COMPONENTDIR = ../..


LIBS += -Wl,--start-group
# this function adds all libraries passed as arguments to the linker and dependency list
defineTest(UseLibs){
    Args=$$ARGS
    for(TheLib, Args) {
        THELIBPATH       = $$COMPONENTDIR/$$TheLib/Build/lib_$$CONFIG_SUFFIX
        PRE_TARGETDEPS  += $$THELIBPATH/lib$${TheLib}.a
        LIBS            += $$THELIBPATH/lib$${TheLib}.a
    }
    export(LIBS)
    export(PRE_TARGETDEPS)
}

####################################
#### stuff for adding libraries ####
####################################
PLATFORM_GUI_COMPONENTDIR = ../../../../../../Platform/Gui/Components

# this function adds all libraries passed as arguments to the linker and dependency list
defineTest(UsePlatformGUILibs){
    Args=$$ARGS
    for(TheLib, Args) {
        THELIBPATH       = $$PLATFORM_GUI_COMPONENTDIR/$$TheLib/Build/lib_$$CONFIG_SUFFIX
        PRE_TARGETDEPS  += $$THELIBPATH/lib$${TheLib}.a
        LIBS            += $$THELIBPATH/lib$${TheLib}.a
    }
    export(LIBS)
    export(PRE_TARGETDEPS)
}

####################################
#### stuff for adding libraries ####
####################################
PLATFORM_COMPONENTDIR = ../../../../../../Platform/Master/Components

# this function adds all libraries passed as arguments to the linker and dependency list
defineTest(UsePlatformLibs){
    Args=$$ARGS
    for(TheLib, Args) {
        THELIBPATH       = $$PLATFORM_COMPONENTDIR/$$TheLib/Build/lib_$$CONFIG_SUFFIX
        PRE_TARGETDEPS  += $$THELIBPATH/lib$${TheLib}.a
        LIBS            += $$THELIBPATH/lib$${TheLib}.a
    }
    export(LIBS)
    export(PRE_TARGETDEPS)
}


####################################
#### stuff for adding libraries ####
####################################
SHARED_COMPONENTDIR = ../../../../../../Himalaya/Shared/Master/Components

# this function adds all libraries passed as arguments to the linker and dependency list
defineTest(UseSharedLibs){
    Args=$$ARGS
#    LIBS += -Wl,--start-group
    for(TheLib, Args) {
        THELIBPATH       = $$SHARED_COMPONENTDIR/$$TheLib/Build/lib_$$CONFIG_SUFFIX
        PRE_TARGETDEPS  += $$THELIBPATH/lib$${TheLib}.a
        LIBS            += $$THELIBPATH/lib$${TheLib}.a
   }
   export(LIBS)
   export(PRE_TARGETDEPS)
}

####################################
#### stuff for adding libraries ####
####################################
PLATFORM_COMPONENTDIR_SCROLLER = ../../../../../../Platform/Gui/Components/kineticscroller/lib

defineTest(UsePlatformScroller){
    PRE_TARGETDEPS  += $$PLATFORM_COMPONENTDIR_SCROLLER/lib$${ARGS}.a
    LIBS            += $$PLATFORM_COMPONENTDIR_SCROLLER/lib$${ARGS}.a

    export(LIBS)
    export(PRE_TARGETDEPS)
}

################# end group
LIBS += -Wl,--end-group

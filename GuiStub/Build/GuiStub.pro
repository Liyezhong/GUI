# build all components recursive

TEMPLATE = subdirs

######## normal build process ########
#
# Make sure your Main.pro is in the last line to ensure correct linking!
#
SUBDIRS  = ../../../../Shared/Master/Build/Shared.pro \
           ../../../Master/Components/HimalayaGuiController/Build/TestHimalayaGuiController.pro \
           ../../../../../Platform/Master/Components/NetCommands/Build/NetCommands.pro \
           ../../../../../Platform/Master/Components/TestStubDataManager/Build/DataManager.pro \
           ../../../../../Platform/Master/Components/ExternalProcessController/Build/TestExternalProcessController.pro \
           ../../../../Shared/Master/Components/HimalayaDataContainer/Build/HimalayaDataContainer.pro \
           ../../../../../Platform/Master/Components/Global/Build/TestGlobal.pro \
           ../../../../../Platform/Master/Components/NetworkComponents/Build/NetworkComponents.pro \
           ../../../../../Platform/Master/Components/StateMachines/Build/StateMachines.pro \
           ../Components/TestHimalayaMasterThread/Build/TestHimalayaMasterThread.pro \
           ../Components/Threads/Build/Threads.pro \
           ../Components/CommandExecuter/Build/CommandExecuter.pro \
           ../Components/Main/Build/Main.pro


CONFIG += ordered

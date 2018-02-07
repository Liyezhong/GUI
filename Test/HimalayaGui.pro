# build all components recursive

TEMPLATE = subdirs
SUBDIRS = ../../../../Platform/Master/Components/Global/Build/Global.pro \
          ../../../../Platform/Master/Components/NetworkComponents/Build/NetworkComponents.pro \
          ../../../../Platform/Master/Components/DataManager/Build/DataManager.pro \
          ../../../../Platform/Gui/Components/Application/Build/Application.pro \
          ../../../../Platform/Gui/Components/KeyBoard/Build/KeyBoard.pro \
          ../../../../Platform/Gui/Components/MainMenu/Build/MainMenu.pro \
          ../../../../Platform/Gui/Components/kineticscroller/qtscroller.pro \
          ../../../Shared/Gui/Components/Images/Build/Images.pro \
          ../../../Shared/Master/Components/HimalayaDataContainer/Build/HimalayaDataContainer.pro \
          ../../../../Platform/Master/Components/NetCommands/Build/NetCommands.pro \
          ../Components/NetworkLayer/Build/NetworkLayer.pro \
          ../Components/Core/Build/Core.pro \
          ../Components/Dashboard/Build/Dashboard.pro \
          ../Components/Programs/Build/Programs.pro \
          ../Components/Reagents/Build/Reagents.pro \
          ../Components/Settings/Build/Settings.pro \
          ../Components/Users/Build/Users.pro \
          ../Components/Dashboard/Test/TestDashboard.pro \
          ../Components/Reagents/Test/TestReagents.pro \
          ../Components/Settings/Test/TestSettings.pro \
          ../Components/Programs/Test/TestPrograms.pro


CONFIG += ordered

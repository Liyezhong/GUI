# build all components recursive

TEMPLATE = subdirs

######## normal build process ########
#
# Make sure your Main.pro is in the last line to ensure correct linking!
#

SUBDIRS = ../../Shared/Master/Components/Global/Build/Global.pro \
          ../../Shared/Master/Components/NetworkComponents/Build/NetworkComponents.pro \
          ../../Shared/Master/Components/DataManager/Build/DataManager.pro \
          ../../Shared/Gui/Components/Application/Build/Application.pro \
          ../../Shared/Gui/Components/KeyBoard/Build/KeyBoard.pro \
          ../../Shared/Gui/Components/MainMenu/Build/MainMenu.pro \
          ../../Shared/Gui/Components/kineticscroller/qtscroller.pro \
          ../../Shared/Common/Components/HimalayaDataContainer/Build/HimalayaDataContainer.pro \
          ../../Shared/Master/Components/NetCommands/Build/NetCommands.pro \
          ../Components/NetworkLayer/Build/NetworkLayer.pro \
          ../Components/Core/Build/Core.pro \
          ../Components/Dashboard/Build/Dashboard.pro \
          ../Components/Programs/Build/Programs.pro \
          ../Components/Reagents/Build/Reagents.pro \
          ../Components/Settings/Build/Settings.pro \
          ../Components/Users/Build/Users.pro
          ../Components/Main/Build/Main.pro


CONFIG += ordered

TRANSLATIONS = ../Components/Main/Translation/Himalaya_de.ts \
               ../Components/Main/Translation/Himalaya_en.ts \
               ../Components/Main/Translation/Himalaya_zh.ts \
               ../Components/Main/Translation/Himalaya_ja.ts \
               ../Components/Main/Translation/Himalaya_pt.ts \
               ../Components/Main/Translation/Himalaya_es.ts \
               ../Components/Main/Translation/Himalaya_ko.ts \
               ../Components/Main/Translation/Himalaya_cs.ts \
               ../Components/Main/Translation/Himalaya_fr.ts \
               ../Components/Main/Translation/Himalaya_hu.ts \
               ../Components/Main/Translation/Himalaya_nl.ts \
               ../Components/Main/Translation/Himalaya_pl.ts \
               ../Components/Main/Translation/Himalaya_tr.ts \
               ../Components/Main/Translation/Himalaya_ru.ts \
               ../Components/Main/Translation/Himalaya_it.ts


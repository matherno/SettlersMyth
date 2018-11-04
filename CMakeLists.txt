cmake_minimum_required(VERSION 3.9)
project(SettlersMyth)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/main.cpp src/SMInputHandler.cpp src/SMInputHandler.h src/SMGameContext.cpp src/SMGameContext.h src/Resources.h src/TimeToLiveActor.cpp src/TimeToLiveActor.h src/HUDHandler.cpp src/HUDHandler.h src/WorldItemPlacementHandler.cpp src/WorldItemPlacementHandler.h src/SMMainMenuContext.cpp src/SMMainMenuContext.h src/SMGameSaveLoad.cpp src/SMGameSaveLoad.h src/SaveLoadDlg.cpp src/SaveLoadDlg.h src/PauseMenuHandler.cpp src/PauseMenuHandler.h src/WorldItemSelectionManager.cpp src/WorldItemSelectionManager.h src/SMSettings.cpp src/SMSettings.h src/SettingsDlg.cpp src/SettingsDlg.h)
add_executable(SettlersMyth ${SOURCE_FILES})

include_directories(./dependencies ./dependencies/TowOff ./src ./dependencies/RaySplay/include ./dependencies/misc)

set(LIB_MATHERNOGL /home/matt/SoftwareProjects/C++/SettlersMyth/dependencies/mathernogl/libMathernoGL.a )
set(LIB_RAYSPLAY /home/matt/SoftwareProjects/C++/SettlersMyth/dependencies/RaySplay/libRaySplayLib.a )
set(LIB_TOWOFF /home/matt/SoftwareProjects/C++/SettlersMyth/dependencies/TowOff/libTowOffLib.a )

target_link_libraries(SettlersMyth ${LIB_TOWOFF} ${LIB_MATHERNOGL} ${LIB_RAYSPLAY} GL GLEW glfw freeimage)
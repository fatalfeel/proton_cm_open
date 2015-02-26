cmake_minimum_required(VERSION 2.6)

project(rt3dapp)

include(../../shared/linux/Proton.cmake)

proton_include_all_components()
proton_use_sdl_audio()
proton_use_zipfilesystem()
proton_use_irrlicht()

set(APP "../source")
include_directories("${APP}")
proton_set_sources("${APP}/App.cpp" "${APP}/GUI/DebugMenu.cpp" "${APP}/GUI/MainMenu.cpp" "${APP}/GUI/Map3Menu.cpp" "${APP}/GUI/MapMenu.cpp" "${APP}/GUI/Mesh3DMenu.cpp" "${APP}/GUI/TerrainMenu.cpp" "${APP}/Component/FPSControlComponent.cpp")

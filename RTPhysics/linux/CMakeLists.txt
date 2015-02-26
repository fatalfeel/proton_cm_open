cmake_minimum_required(VERSION 2.6)

project(rtphysics)

include(../../shared/linux/Proton.cmake)

proton_include_all_components()
proton_use_sdl_audio()
proton_use_bullet()

set(APP "../source")
include_directories("${APP}")
proton_set_sources("${APP}/App.cpp" "${APP}/GUI/DebugMenu.cpp" "${APP}/GUI/MainMenu.cpp" "${APP}/GUI/PhysicsHelloMenu.cpp" "${APP}/Component/FPSControlComponent.cpp")

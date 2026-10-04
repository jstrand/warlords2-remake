cmake_minimum_required(VERSION 3.16)
project(warlords2 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  add_compile_options(-Wall -Wextra -Wno-unused-parameter -Wno-sign-compare)
endif()

# The rules core and the computer player: no SDL, so the tests link it alone.
file(GLOB CORE_SOURCES CONFIGURE_DEPENDS src/util/*.cpp src/warlords/*.cpp src/warlords/ai/*.cpp)
add_library(w2core STATIC ${CORE_SOURCES})
target_include_directories(w2core PUBLIC src)
find_package(Threads REQUIRED)
target_link_libraries(w2core PUBLIC Threads::Threads)

# The game: the slim layer over SDL2, the front end and the dialogs.
find_package(SDL2 CONFIG QUIET)
if(NOT SDL2_FOUND)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)
endif()
file(GLOB APP_SOURCES CONFIGURE_DEPENDS src/platform/*.cpp src/ui/*.cpp src/front/*.cpp src/main.cpp)
add_executable(warlords2 ${APP_SOURCES})
target_link_libraries(warlords2 PRIVATE w2core)
if(TARGET SDL2::SDL2)
  target_link_libraries(warlords2 PRIVATE SDL2::SDL2)
else()
  target_link_libraries(warlords2 PRIVATE PkgConfig::SDL2)
endif()

# The Roland recordings are Ogg Vorbis: without libvorbisfile the game still
# builds, and plays the AdLib music only.
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
  pkg_check_modules(VORBISFILE QUIET IMPORTED_TARGET vorbisfile)
endif()
if(VORBISFILE_FOUND)
  target_link_libraries(warlords2 PRIVATE PkgConfig::VORBISFILE)
  target_compile_definitions(warlords2 PRIVATE W2_VORBIS=1)
else()
  message(STATUS "libvorbisfile not found: the recorded music is left out")
endif()

# Tests: the rules suite and the Lua-vs-C++ trace.
add_executable(w2test test/run.cpp)
target_link_libraries(w2test PRIVATE w2core)
add_executable(w2trace test/trace.cpp)
target_link_libraries(w2trace PRIVATE w2core)
add_executable(w2fm test/fmrender.cpp)
target_link_libraries(w2fm PRIVATE w2core)

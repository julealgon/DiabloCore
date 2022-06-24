cmake_minimum_required(VERSION 3.13)

if(POLICY CMP0092)
  cmake_policy(SET CMP0092 NEW)
endif()
if(POLICY CMP0083)
  cmake_policy(SET CMP0083 NEW)
endif()
if(POLICY CMP0111)
  cmake_policy(SET CMP0111 NEW)
endif()

include(CMakeDependentOption)
include(CMake/out_of_tree.cmake)
include(CMake/genex.cmake)

option(TSAN "Enable thread sanitizer" OFF)
DEBUG_OPTION(DEBUG "Enable debug mode in engine")
option(DISABLE_LTO "Disable link-time optimization (by default enabled in release mode)" OFF)
option(PIE "Generate position-independent code" OFF)
option(BINARY_RELEASE "Enable options for binary release" OFF)
option(NIGHTLY_BUILD "Enable options for nightly build" OFF)

RELEASE_OPTION(CPACK "Configure CPack")

if(BINARY_RELEASE OR CMAKE_BUILD_TYPE STREQUAL "Release")
  set(BINARY_RELEASE ON)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "")
  set(DIST ON)
  set(CPACK ON)
endif()

if(NIGHTLY_BUILD OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  set(NIGHTLY_BUILD ON)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "")
  set(DIST ON)
  set(CPACK ON)
endif()

option(DIABLOCORE_SYSTEM_SDL_AUDIOLIB "Use system-provided SDL_audiolib" OFF)
cmake_dependent_option(DIABLOCORE_STATIC_SDL_AUDIOLIB "Link static SDL_audiolib" OFF
                      "DIABLOCORE_SYSTEM_SDL_AUDIOLIB AND NOT DIST" ON)

option(DIABLOCORE_SYSTEM_LIBFMT "Use system-provided libfmt" ON)
cmake_dependent_option(DIABLOCORE_STATIC_LIBFMT "Link static libfmt" OFF
                       "DIABLOCORE_SYSTEM_LIBFMT AND NOT DIST" ON)

if(NOT VERSION_NUM)
  include(CMake/git.cmake)
  get_git_tag(VERSION_NUM)
  if (NOT "${VERSION_NUM}" STREQUAL "")
    string(REGEX MATCH "([0-9]+\\.[0-9]+\\.[0-9]+)" VERSION_NUM ${VERSION_NUM} )
  endif()
  get_git_commit_hash(GIT_COMMIT_HASH)
  if(NOT VERSION_SUFFIX)
    set(VERSION_SUFFIX "$<$<NOT:$<CONFIG:Release>>:-${GIT_COMMIT_HASH}>")
  endif()
endif()

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

if(VERSION_NUM MATCHES untagged)
  project(DiabloCore
    LANGUAGES C CXX)
else()
  project(DiabloCore
    VERSION ${VERSION_NUM}
    LANGUAGES C CXX)
endif()

# Not a genexp because CMake doesn't support it
# https://gitlab.kitware.com/cmake/cmake/-/issues/20546
if(NOT DISABLE_LTO)
  # LTO if supported:
  include(CheckIPOSupported)
  check_ipo_supported(RESULT is_ipo_supported OUTPUT lto_error)
  if(is_ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL ON)
  endif()
endif()

list(APPEND CMAKE_MODULE_PATH "${DiabloCore_SOURCE_DIR}/CMake")

if(PIE)
  set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # for clang-tidy
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

if(DIABLOCORE_SYSTEM_LIBFMT)
  find_package(fmt 7.0.0 QUIET)
  if(fmt_FOUND)
    message("-- Found fmt ${fmt_VERSION}")
  else()
    message("-- Suitable system fmt package not found, will use fmt from source")
  endif()
endif()
if(NOT fmt_FOUND)
  add_subdirectory(3rdParty/libfmt)
endif()

find_package(SDL2 REQUIRED)
if(TARGET SDL2::SDL2)
  set(SDL2_MAIN SDL2::SDL2main)
elseif(TARGET SDL2::SDL2-static)
  # On some distros, only the SDL2::SDL2-static target is available.
  # Alias to SDL2::SDL2 because some finder scripts may refer to SDL2::SDL2.
  if(CMAKE_VERSION VERSION_LESS "3.18")
    # Aliasing local targets is not supported on CMake < 3.18, so make it global.
    set_target_properties(SDL2::SDL2-static PROPERTIES IMPORTED_GLOBAL TRUE)
  endif()
  add_library(SDL2::SDL2 ALIAS SDL2::SDL2-static)
  set(SDL2_MAIN SDL2::SDL2main)
else()
  # Assume an older Debian derivate that comes with an sdl2-config.cmake
  # that only defines `SDL2_LIBRARIES` (as -lSDL2) and `SDL2_INCLUDE_DIRS`.
  add_library(SDL2_lib INTERFACE)
  target_link_libraries(SDL2_lib INTERFACE ${SDL2_LIBRARIES})
  target_include_directories(SDL2_lib INTERFACE ${SDL2_INCLUDE_DIRS})
  # Can't define an INTERFACE target with ::, so alias instead
  add_library(SDL2::SDL2 ALIAS SDL2_lib)
endif()
find_package(SDL2_ttf REQUIRED)

if(DIABLOCORE_SYSTEM_SDL_AUDIOLIB)
  find_package(SDL_audiolib REQUIRED)
else()
  add_subdirectory(3rdParty/SDL_audiolib)
endif()

add_library(smacker STATIC
  3rdParty/libsmacker/smacker.c)
target_include_directories(smacker PUBLIC 3rdParty/libsmacker)

add_subdirectory(3rdParty/find_steam_game)
add_subdirectory(3rdParty/simpleini)

add_library(StormLib STATIC
  3rdParty/StormLib/src/FileStream.cpp
  3rdParty/StormLib/src/SBaseCommon.cpp
  3rdParty/StormLib/src/SBaseFileTable.cpp
  3rdParty/StormLib/src/SBaseSubTypes.cpp
  3rdParty/StormLib/src/SCompression.cpp
  3rdParty/StormLib/src/SFileExtractFile.cpp
  3rdParty/StormLib/src/SFileFindFile.cpp
  3rdParty/StormLib/src/SFileGetFileInfo.cpp
  3rdParty/StormLib/src/SFileOpenArchive.cpp
  3rdParty/StormLib/src/SFileOpenFileEx.cpp
  3rdParty/StormLib/src/SFileReadFile.cpp)

# Enable Unicode for StormLib wchar_t* file APIs
target_compile_definitions(StormLib PRIVATE -DUNICODE -D_UNICODE)

add_library(PKWare STATIC
  3rdParty/PKWare/explode.cpp
  3rdParty/PKWare/implode.cpp)
target_include_directories(PKWare PUBLIC 3rdParty/PKWare)

set(libdiablocore_SRCS
  Source/appfat.cpp
  Source/automap.cpp
  Source/capture.cpp
  Source/codec.cpp
  Source/control.cpp
  Source/cursor.cpp
  Source/dead.cpp
  Source/debug.cpp
  Source/diablo.cpp
  Source/doom.cpp
  Source/drlg_l1.cpp
  Source/drlg_l2.cpp
  Source/drlg_l3.cpp
  Source/drlg_l4.cpp
  Source/dx.cpp
  Source/encrypt.cpp
  Source/engine.cpp
  Source/error.cpp
  Source/gamemenu.cpp
  Source/gendung.cpp
  Source/gmenu.cpp
  Source/help.cpp
  Source/hwcursor.cpp
  Source/init.cpp
  Source/interfac.cpp
  Source/inv.cpp
  Source/itemdat.cpp
  Source/items.cpp
  Source/lighting.cpp
  Source/loadsave.cpp
  Source/menu.cpp
  Source/minitext.cpp
  Source/misdat.cpp
  Source/missiles.cpp
  Source/monstdat.cpp
  Source/monster.cpp
  Source/movie.cpp
  Source/mpqapi.cpp
  Source/msg.cpp
  Source/multi.cpp
  Source/nthread.cpp
  Source/objdat.cpp
  Source/objects.cpp
  Source/options.cpp
  Source/pack.cpp
  Source/palette.cpp
  Source/path.cpp
  Source/pfile.cpp
  Source/player.cpp
  Source/plrmsg.cpp
  Source/portal.cpp
  Source/quests.cpp
  Source/restrict.cpp
  Source/scrollrt.cpp
  Source/setmaps.cpp
  Source/sha.cpp
  Source/spelldat.cpp
  Source/spells.cpp
  Source/stores.cpp
  Source/sync.cpp
  Source/textdat.cpp
  Source/themes.cpp
  Source/tmsg.cpp
  Source/town.cpp
  Source/towners.cpp
  
  Source/trigs.cpp
  Source/controls/menu_controls.cpp
  Source/controls/keymapper.cpp
  Source/engine/animationinfo.cpp
  
  Source/engine/load_cel.cpp
  Source/engine/random.cpp
  Source/engine/render/automap_render.cpp
  Source/engine/render/cel_render.cpp
  Source/engine/render/cl2_render.cpp
  Source/engine/render/dun_render.cpp
  Source/engine/render/text_render.cpp
  Source/engine/surface.cpp
  
  Source/qol/common.cpp
  Source/qol/monhealthbar.cpp
  Source/qol/xpbar.cpp
  Source/qol/itemlabels.cpp
  Source/utils/console.cpp
  Source/utils/display.cpp
  Source/utils/file_util.cpp
  
  Source/effects.cpp
  Source/sound.cpp
  Source/utils/push_aulib_decoder.cpp
  Source/utils/soundsample.cpp
  
  Source/utils/paths.cpp
  Source/utils/sdl_bilinear_scale.cpp
  Source/utils/sdl_thread.cpp
  Source/DiabloUI/art.cpp
  Source/DiabloUI/art_draw.cpp
  Source/DiabloUI/button.cpp
  Source/DiabloUI/credits.cpp
  Source/DiabloUI/credits_lines.cpp
  Source/DiabloUI/diabloui.cpp
  Source/DiabloUI/dialogs.cpp
  Source/DiabloUI/errorart.cpp
  Source/DiabloUI/fonts.cpp
  Source/DiabloUI/mainmenu.cpp
  Source/DiabloUI/progress.cpp
  Source/DiabloUI/scrollbar.cpp
  
  Source/DiabloUI/selhero.cpp
  Source/DiabloUI/selok.cpp
  Source/DiabloUI/selyesno.cpp
  Source/DiabloUI/support_lines.cpp
  Source/DiabloUI/text_draw.cpp
  Source/DiabloUI/title.cpp
  Source/DiabloUI/ttf_render_wrapped.cpp
  Source/panels/charpanel.cpp
  Source/panels/mainpanel.cpp

  Source/dvlnet/loopback.cpp
    
  Source/storm/storm.cpp
  
  Source/storm/storm_net.cpp
  Source/storm/storm_sdl_rw.cpp
  Source/storm/storm_svid.cpp
  Source/miniwin/misc_msg.cpp)

set(BIN_TARGET diablocore)

add_library(libdiablocore OBJECT ${libdiablocore_SRCS})
add_executable(${BIN_TARGET}
  WIN32
  Source/main.cpp
  Source/diablocore.exe.manifest
  Packaging/resources/CharisSILB.ttf
  Packaging/windows/diablocore.rc)
target_link_libraries(${BIN_TARGET} PRIVATE libdiablocore)

# Copy the font and diablocore.mpq to the build directory to it works from the build directory
file(COPY "Packaging/resources/CharisSILB.ttf" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
file(COPY "Packaging/resources/diablocore.mpq" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

# Use file GENERATE instead of configure_file because configure_file
# does not support generator expressions.
get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(is_multi_config)
  set(CONFIG_PATH $<CONFIG>/config.h)
  target_include_directories(libdiablocore PUBLIC ${CMAKE_BINARY_DIR}/$<CONFIG>)
else()
  set(CONFIG_PATH config.h)
endif()
file(GENERATE OUTPUT ${CONFIG_PATH} CONTENT
"#pragma once
#define PROJECT_NAME \"${PROJECT_NAME}\"
#define PROJECT_VERSION \"${PROJECT_VERSION}${VERSION_SUFFIX}\"
#define PROJECT_VERSION_MAJOR ${PROJECT_VERSION_MAJOR}
#define PROJECT_VERSION_MINOR ${PROJECT_VERSION_MINOR}
#define PROJECT_VERSION_PATCH ${PROJECT_VERSION_PATCH}
")

target_include_directories(libdiablocore PUBLIC
  Source
  ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(libdiablocore PUBLIC
Threads::Threads)

target_link_libraries(libdiablocore PUBLIC
  PKWare
  StormLib
  smacker
  simpleini)

target_link_libraries(libdiablocore PUBLIC find_steam_game)

target_link_libraries(libdiablocore PUBLIC fmt::fmt)

genex_for_option(DEBUG)
target_compile_definitions(libdiablocore PUBLIC "$<${DEBUG_GENEX}:_DEBUG>")

# Defines with value
foreach(
  def_name
  DEFAULT_WIDTH
  DEFAULT_HEIGHT
  TTF_FONT_DIR
  TTF_FONT_NAME
  REMAP_KEYBOARD_KEYS
)
  if(DEFINED ${def_name})
    list(APPEND def_list ${def_name}=${${def_name}})
  endif()
endforeach(def_name)

if(TSAN)
  target_compile_options(libdiablocore PUBLIC -fsanitize=thread)
  target_link_libraries(libdiablocore PUBLIC -fsanitize=thread)
endif()

target_link_libraries(libdiablocore PUBLIC
  SDL2::SDL2
  ${SDL2_MAIN}
  SDL2::SDL2_ttf)

target_link_libraries(libdiablocore PUBLIC SDL_audiolib)

target_compile_definitions(libdiablocore PUBLIC ${def_list})

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  target_link_libraries(libdiablocore PUBLIC "$<$<NOT:$<CONFIG:Debug>>:-static-libgcc;-static-libstdc++>")
endif()

target_link_libraries(libdiablocore PUBLIC shlwapi wsock32 ws2_32 wininet)

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  target_compile_options(libdiablocore PUBLIC $<$<CONFIG:Debug>:-gstabs>)
endif()

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  # Change __FILE__ to only show the path relative to the project folder
  get_target_property(libdiablocore_SRCS ${BIN_TARGET} SOURCES)
  foreach(SOURCE_FILE ${libdiablocore_SRCS})
    set_source_files_properties(${SOURCE_FILE} PROPERTIES
      COMPILE_DEFINITIONS __FILE__="${SOURCE_FILE}"
    )
  endforeach(SOURCE_FILE)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-builtin-macro-redefined")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-builtin-macro-redefined")

  # Note: For Valgrind support.
  genex_for_option(DEBUG)
  target_compile_options(libdiablocore PUBLIC $<${DEBUG_GENEX}:-fno-omit-frame-pointer>)

  # Warnings for devilutionX
  target_compile_options(libdiablocore PUBLIC -Wall -Wextra -Wno-unused-parameter)

  # For ARM and other default unsigned char platforms
  target_compile_options(libdiablocore PUBLIC -fsigned-char)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  target_compile_options(libdiablocore PUBLIC "/W3")
  target_compile_definitions(libdiablocore PUBLIC _CRT_SECURE_NO_WARNINGS)
  target_compile_options(libdiablocore PUBLIC "/Zc:__cplusplus")
endif()

if(CPACK)
  if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(SDL2_WIN32_DLLS_DIR "${CMAKE_BINARY_DIR}")
  else()
    set(SDL2_WIN32_DLLS_DIR "${SDL2_EXEC_PREFIX}/bin")
  endif()
  set(SDL2_WIN32_LICENSES_DIR "${PROJECT_SOURCE_DIR}/Packaging/resources")
  
  file(GLOB SDL2_WIN32_ALL_DLLS
    LIST_DIRECTORIES false
    "${SDL2_WIN32_DLLS_DIR}/*.dll")
  file(GLOB SDL2_WIN32_ALL_LICENSES
    LIST_DIRECTORIES false
    "${SDL2_WIN32_LICENSES_DIR}/LICENSE*.txt"
    "${SDL2_WIN32_LICENSES_DIR}/README*.txt")
  
  set(CPACK_PACKAGE_FILE_NAME "diablocore")
  set(CPACK_PACKAGE_NAME ${project_name})
  set(CPACK_GENERATOR "ZIP")
  set(CPACK_STRIP_FILES TRUE)
  install(TARGETS ${BIN_TARGET} DESTINATION .)
  install(FILES "${PROJECT_SOURCE_DIR}/Packaging/windows/README.txt"
    DESTINATION "."
  )
  install(FILES "${PROJECT_SOURCE_DIR}/Packaging/resources/CharisSILB.ttf"
    DESTINATION "."
  )
  install(FILES "${PROJECT_SOURCE_DIR}/Packaging/resources/diablocore.mpq"
    DESTINATION "."
  )
  
  foreach(_SDL2_WIN32_DLL_PATH ${SDL2_WIN32_ALL_DLLS})
    install(FILES "${_SDL2_WIN32_DLL_PATH}"
      DESTINATION "."
    )
  endforeach()
  
  foreach(_SDL2_WIN32_LICENSE_PATH ${SDL2_WIN32_ALL_LICENSES})
    install(FILES "${_SDL2_WIN32_LICENSE_PATH}"
      DESTINATION "LICENSE"
    )
  endforeach()

  set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
  set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
  set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
  include(CPack)
endif()

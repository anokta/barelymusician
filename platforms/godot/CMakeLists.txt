set(CMAKE_FOLDER external)

FetchContent_Declare(
  godotcpp
  GIT_REPOSITORY https://github.com/godotengine/godot-cpp.git
  GIT_TAG godot-4.5-stable
)
if(EMSCRIPTEN)
  set(GODOTCPP_THREADS OFF CACHE BOOL "" FORCE)
endif()
FetchContent_MakeAvailable(godotcpp)
target_compile_options(
  godot-cpp PUBLIC
  -w
)
include_directories(${godotcpp_SOURCE_DIR}/src)

set(CMAKE_FOLDER platforms)

add_library(
  barelymusiciangodot SHARED
  engine.cpp
  engine.h
  instrument.cpp
  instrument.h
  performer.cpp
  performer.h
  resources.cpp
  resources.h
  godot.cpp
)
target_compile_definitions(
  barelymusiciangodot PUBLIC
  BARELY_EXPORT
)
target_include_directories(
  barelymusiciangodot PUBLIC
  ${GODOT_CPP_INCLUDE_DIRS}
)
target_link_libraries(
  barelymusiciangodot PUBLIC
  barelymusician_mt
  godot-cpp
)
if(EMSCRIPTEN)
  set_target_properties(
    barelymusician_mt PROPERTIES
    POSITION_INDEPENDENT_CODE ON
  )
  target_link_options(
    barelymusiciangodot PRIVATE
    -sSIDE_MODULE=1
    -lc++
    -lc++abi
  )
endif()

function(package_barelymusiciangodot)
  set(ADDON_BIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/project/addons/barelymusician/bin)
  if(ANDROID)
    set(ADDON_DEST_PATH ${ADDON_BIN_DIR}/android/${ANDROID_ABI}/libbarelymusiciangodot.so)
  elseif(APPLE)
    set(ADDON_DEST_PATH ${ADDON_BIN_DIR}/macos/libbarelymusiciangodot.dylib)
  elseif(EMSCRIPTEN)
    set(ADDON_DEST_PATH ${ADDON_BIN_DIR}/web/barelymusiciangodot.wasm)
  elseif(WIN32)
    set(ADDON_DEST_PATH ${ADDON_BIN_DIR}/windows/barelymusiciangodot.dll)
  elseif(UNIX)
    set(ADDON_DEST_PATH ${ADDON_BIN_DIR}/linux/libbarelymusiciangodot.so)
  else()
    message(FATAL_ERROR "Unsupported Godot platform")
  endif()

  add_custom_command(
    TARGET barelymusiciangodot POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${ADDON_BIN_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:barelymusiciangodot> ${ADDON_DEST_PATH}
    COMMENT "Copying BarelyMusician Godot plugin to addon directory"
  )
endfunction()
package_barelymusiciangodot()

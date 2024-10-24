add_library(
  barelymusicianunity SHARED
  empty.cpp
)
target_compile_definitions(
  barelymusicianunity
  PUBLIC BARELYMUSICIAN_EXPORTS
)
target_link_libraries(
  barelymusicianunity PUBLIC
  barelymusician
  barelymusician_arpeggiator
  barelymusician_repeater
)
if(MSVC)
  target_sources(
    barelymusicianunity PUBLIC
    unity_windows.def
  )
endif()
if(ANDROID)
  target_link_options(
    barelymusicianunity PUBLIC
    -ldl
    -lm
    "-Wl --version-script unity_android.lds"
    -z defs
  )
endif()

function(package_barelymusicianunity)
  set(PLUGINS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Assets/BarelyMusician/Plugins)
  if(ANDROID)
    set(PLUGIN_DEST_PATH ${PLUGINS_DIR}/Android/libs/${ANDROID_ABI}/libbarelymusicianunity.so)
  elseif(APPLE)
    set(PLUGIN_DEST_PATH ${PLUGINS_DIR}/x86_64/libbarelymusicianunity.dylib)
  elseif(WIN32)
    set(PLUGIN_DEST_PATH ${PLUGINS_DIR}/x86_64/barelymusicianunity.dll)
  elseif(UNIX)
    set(PLUGIN_DEST_PATH ${PLUGINS_DIR}/x86_64/libbarelymusicianunity.so)
  else()
    message(FATAL_ERROR "Unsupported Unity platform")
  endif()

  add_custom_command(
    TARGET barelymusicianunity POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:barelymusicianunity> ${PLUGIN_DEST_PATH}
  )
endfunction()
package_barelymusicianunity()

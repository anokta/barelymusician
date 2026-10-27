add_library(
  barelymusicianunity ${BARELYMUSICIAN_PLUGIN_LIBRARY_TYPE}
  unity.cpp
)
target_compile_definitions(
  barelymusicianunity PUBLIC
  BARELY_EXPORT
)
target_link_libraries(
  barelymusicianunity PUBLIC
  barelymusician
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
  if(IOS)
    package_barelymusician_ios_plugin(
      barelymusicianunity
      "${PLUGINS_DIR}/iOS/libbarelymusicianunity.xcframework"
    )
  else()
    if(ANDROID)
      set(PLUGIN_DEST_PATH ${PLUGINS_DIR}/Android/libs/${ANDROID_ABI}/libbarelymusicianunity.so)
    elseif(APPLE)
      set(PLUGIN_DEST_PATH ${PLUGINS_DIR}/x86_64/libbarelymusicianunity.dylib)
      set(SIGN_COMMAND COMMAND codesign --force --sign - ${PLUGIN_DEST_PATH})
    elseif(WIN32)
      set(PLUGIN_DEST_PATH ${PLUGINS_DIR}/x86_64/barelymusicianunity.dll)
    elseif(UNIX)
      set(PLUGIN_DEST_PATH ${PLUGINS_DIR}/x86_64/libbarelymusicianunity.so)
    else()
      message(FATAL_ERROR "Unsupported Unity platform")
    endif()
    add_custom_command(
      TARGET barelymusicianunity POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E make_directory ${PLUGINS_DIR}
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:barelymusicianunity> ${PLUGIN_DEST_PATH}
      ${SIGN_COMMAND}
    )
  endif()
endfunction()
package_barelymusicianunity()

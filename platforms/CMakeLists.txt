set(CMAKE_FOLDER platforms)

if(IOS)
  set(BARELYMUSICIAN_PLUGIN_LIBRARY_TYPE STATIC)
else()
  set(BARELYMUSICIAN_PLUGIN_LIBRARY_TYPE SHARED)
endif()

function(package_barelymusician_ios_plugin target_name dest_path)
  get_target_property(plugin_dependencies ${target_name} LINK_LIBRARIES)
  set(dependency_archives)
  foreach(dependency IN LISTS plugin_dependencies)
    list(APPEND dependency_archives "$<TARGET_FILE:${dependency}>")
  endforeach()
  if(CMAKE_OSX_SYSROOT MATCHES "[Ss]imulator")
    set(ios_sdk_variant simulator)
  else()
    set(ios_sdk_variant device)
  endif()
  set(ios_archive_dir "${CMAKE_BINARY_DIR}/package/ios/${ios_sdk_variant}")
  set(ios_archive "${ios_archive_dir}/${target_name}.a")
  add_custom_command(
    TARGET ${target_name} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${ios_archive_dir}"
    COMMAND xcrun libtool -static -o "${ios_archive}"
    "$<TARGET_FILE:${target_name}>"
    ${dependency_archives}
  )
  if(ios_sdk_variant STREQUAL "simulator")
    get_filename_component(dest_dir "${dest_path}" DIRECTORY)
    add_custom_command(
      TARGET ${target_name} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E make_directory "${dest_dir}"
      COMMAND ${CMAKE_COMMAND} -E rm -rf "${dest_path}"
      COMMAND xcodebuild -create-xcframework
      -library "${CMAKE_BINARY_DIR}/../device/package/ios/device/${target_name}.a"
      -library "${ios_archive}"
      -output "${dest_path}"
    )
  endif()
endfunction()

include_directories(.)

if(ENABLE_DAISY)
  add_subdirectory(daisy)
endif()

if(ENABLE_GODOT)
  add_subdirectory(godot)
endif()

if(ENABLE_UNITY)
  add_subdirectory(unity)
endif()

if(ENABLE_VST)
  add_subdirectory(vst)
endif()

if(ENABLE_WEB)
  add_subdirectory(web)
endif()

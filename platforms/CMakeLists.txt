set(CMAKE_FOLDER platforms)

include_directories(.)

if(ENABLE_DAISY)
  add_subdirectory(daisy)
endif()

if(ENABLE_UNITY)
  add_subdirectory(unity)
endif()

if(ENABLE_VST)
  add_subdirectory(vst)
endif()

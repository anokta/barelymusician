set(CMAKE_FOLDER platforms)

if(ENABLE_DAISY)
  add_subdirectory(daisy)
endif()

if(ENABLE_UNITY)
  add_subdirectory(unity)
endif()

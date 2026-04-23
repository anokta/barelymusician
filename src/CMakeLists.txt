set(CMAKE_FOLDER src)

include_directories(.)

function(add_barelymusician_target target_name)
  add_library(${target_name} STATIC
    barelymusician.cpp
    ${CMAKE_SOURCE_DIR}/include/barelymusician.h
  )
  target_compile_definitions(
    ${target_name} PUBLIC
    BARELY_EXPORT
  )
  if(MSVC)
    target_compile_options(
      ${target_name} PUBLIC
      /fp:fast
      $<$<CONFIG:RELEASE>: /O2 /Ob2 /Oi /Ot /Oy>
    )
  else()
    target_compile_options(
      ${target_name} PUBLIC
      -ffast-math -fno-math-errno
      $<$<CONFIG:RELEASE>: -O3 -finline-functions -fomit-frame-pointer>
    )
  endif()
  set_target_properties(
    ${target_name} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
  )
  if("${target_name}" MATCHES "_mt$")
    set_target_properties(
      ${target_name} PROPERTIES
      MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
  endif()
endfunction()
add_barelymusician_target(barelymusician)
if(ENABLE_GODOT)
  add_barelymusician_target(barelymusician_mt)
endif()

if(ENABLE_BENCHMARKS)
  add_executable(
    barelymusician_benchmark
    barelymusician_benchmark.cpp
  )
  target_link_libraries(
    barelymusician_benchmark
    barelymusician
    benchmark_main
  )
endif()

if(ENABLE_TESTS)
  add_executable(
    barelymusician_test
    barelymusician_test.cpp
  )
  target_link_libraries(
    barelymusician_test
    barelymusician
    gmock
    gtest_main
  )
  gtest_discover_tests(barelymusician_test)
endif()

function(barelymusician_target_sources)
  target_sources(barelymusician PRIVATE ${ARGN})
  if(ENABLE_GODOT)
    target_sources(barelymusician_mt PRIVATE ${ARGN})
  endif()
endfunction()

add_subdirectory(core)
add_subdirectory(dsp)
add_subdirectory(engine)


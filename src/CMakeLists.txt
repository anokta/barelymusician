set(CMAKE_FOLDER src)

include_directories(.)

add_library(
  barelymusician STATIC
  barelymusician.cpp
  ${CMAKE_SOURCE_DIR}/include/barelymusician.h
)
set_target_properties(
  barelymusician PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
target_compile_definitions(
  barelymusician PUBLIC
  BARELY_EXPORT
  BARELY_ID_INDEX_BIT_COUNT=20
  BARELY_MAX_FRAME_COUNT=${MAX_FRAME_COUNT}
  BARELY_MAX_INSTRUMENT_COUNT=${MAX_INSTRUMENT_COUNT}
  BARELY_MAX_NOTE_COUNT=${MAX_NOTE_COUNT}
  BARELY_MAX_PERFORMER_COUNT=${MAX_PERFORMER_COUNT}
  BARELY_MAX_SLICE_COUNT=${MAX_SLICE_COUNT}
  BARELY_MAX_TASK_COUNT=${MAX_TASK_COUNT}
  BARELY_MAX_VOICE_COUNT=${MAX_VOICE_COUNT}
)
if(MSVC)
  target_compile_options(
    barelymusician PUBLIC
    /fp:fast
    $<$<CONFIG:RELEASE>: /O2 /Ob2 /Oi /Ot /Oy>
  )
else()
  target_compile_options(
    barelymusician PUBLIC
    -ffast-math -fno-math-errno
    $<$<CONFIG:RELEASE>: -O3 -finline-functions -fomit-frame-pointer>
  )
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

add_subdirectory(core)
add_subdirectory(dsp)
add_subdirectory(engine)

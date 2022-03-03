add_library(
  barelymusician
  barelymusician.cpp
)
target_compile_definitions(
  barelymusician
  PUBLIC BARELYMUSICIAN_EXPORTS
)
target_link_libraries(
  barelymusician
  barelymusician_headeronly
  event
  find_or_null
  instrument
  mutable_data
  parameter
  sequence
  transport
)

add_library(
  barelymusician_headeronly INTERFACE
  barelymusician.h
)

add_library(
  event INTERFACE
  event.h
)

add_library(
  event_queue
  event_queue.cpp
  event_queue.h
)
target_link_libraries(
  event_queue
  event
)

add_library(
  find_or_null INTERFACE
  find_or_null.h
)

add_library(
  gain_processor
  gain_processor.cpp
  gain_processor.h
)

add_library(
  instrument
  instrument.cpp
  instrument.h
)
target_link_libraries(
  instrument
  barelymusician_headeronly
  gain_processor
  event
  event_queue
  parameter
)

add_library(
  mutable_data INTERFACE
  mutable_data.h
)

add_library(
  parameter
  parameter.cpp
  parameter.h
)
target_link_libraries(
  parameter
  barelymusician_headeronly
)

add_library(
  sequence
  sequence.cpp
  sequence.h
)
target_link_libraries(
  sequence
  barelymusician_headeronly
  event
  find_or_null
)

add_library(
  transport
  transport.cpp
  transport.h
)

add_executable(
  find_or_null_test
  find_or_null_test.cpp
)
target_link_libraries(
  find_or_null_test
  find_or_null
  gtest_main
)
gtest_discover_tests(find_or_null_test)

add_executable(
  parameter_test
  parameter_test.cpp
)
target_link_libraries(
  parameter_test
  parameter
  gtest_main
)
gtest_discover_tests(parameter_test)

add_executable(
  transport_test
  transport_test.cpp
)
target_link_libraries(
  transport_test
  transport
  gmock
  gtest_main
)
gtest_discover_tests(transport_test)

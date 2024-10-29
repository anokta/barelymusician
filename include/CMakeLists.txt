# TODO(#141): Remove this file altogether.
add_library(
  barelycomposer_headeronly INTERFACE
  barelycomposer.h
)
target_link_libraries(
  barelycomposer_headeronly INTERFACE
  barelymusician_headeronly
)

add_library(
  barelymusician_headeronly INTERFACE
  barelymusician.h
)

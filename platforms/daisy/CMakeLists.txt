set(BARELYMUSICIAN_DAISY_TARGET barelymusiciandaisy)
set(BARELYMUSICIAN_DAISY_LINKER_SCRIPT ${libdaisy_SOURCE_DIR}/core/STM32H750IB_sram.lds)

add_executable(
  ${BARELYMUSICIAN_DAISY_TARGET}
  daisy.cpp
)
set_target_properties(
  ${BARELYMUSICIAN_DAISY_TARGET} PROPERTIES
  SUFFIX ".elf"
  LINK_DEPENDS ${BARELYMUSICIAN_DAISY_LINKER_SCRIPT}
)
target_compile_options(
  ${BARELYMUSICIAN_DAISY_TARGET} PUBLIC
  -Os
  -Wall
  -Wno-attributes
  -Wno-strict-aliasing
  -Wno-maybe-uninitialized
  -Wno-missing-attributes
  -Wno-stringop-overflow
  -Wno-error=reorder
  -Wno-error=sign-compare
  -fexceptions
  -DQ_DONT_USE_THREADS=1
  $<$<COMPILE_LANGUAGE:CXX>:-Wno-register>
)
target_link_libraries(
  ${BARELYMUSICIAN_DAISY_TARGET} PUBLIC
  barelymusician
  barelymusician_oscillator
  barelymusician_ultimate_instrument
  daisy
)
target_link_options(
  ${BARELYMUSICIAN_DAISY_TARGET} PUBLIC
  -T ${BARELYMUSICIAN_DAISY_LINKER_SCRIPT}
  -Wl,-Map=${BARELYMUSICIAN_DAISY_TARGET}.map,--cref
  -Wl,--check-sections
  -Wl,--unresolved-symbols=report-all
  -Wl,--warn-common
  -Wl,--warn-section-align
  -Wl,--print-memory-usage
)

add_custom_command(
  TARGET ${BARELYMUSICIAN_DAISY_TARGET} POST_BUILD
  COMMAND ${CMAKE_OBJCOPY}
  ARGS -O ihex
  -S ${BARELYMUSICIAN_DAISY_TARGET}.elf
  ${BARELYMUSICIAN_DAISY_TARGET}.hex
  BYPRODUCTS
  ${BARELYMUSICIAN_DAISY_TARGET}.hex
  COMMENT "Generating HEX image"
  VERBATIM
)
add_custom_command(
  TARGET ${BARELYMUSICIAN_DAISY_TARGET} POST_BUILD
  COMMAND ${CMAKE_OBJCOPY}
  ARGS -O binary
  -S ${BARELYMUSICIAN_DAISY_TARGET}.elf
  ${BARELYMUSICIAN_DAISY_TARGET}.bin
  BYPRODUCTS
  ${BARELYMUSICIAN_DAISY_TARGET}.bin
  COMMENT "Generating binary image"
  VERBATIM
)

add_custom_target(
  ${BARELYMUSICIAN_DAISY_TARGET}_program_dfu
  COMMAND dfu-util -a 0 -s 0x90040000:leave -D ${BARELYMUSICIAN_DAISY_TARGET}.bin -d ,0483:df11
)

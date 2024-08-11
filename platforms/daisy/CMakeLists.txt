SET(BARELY_DAISY_TARGET barelymusiciandaisy)
set(BARELY_DAISY_LINKER_SCRIPT ${libdaisy_SOURCE_DIR}/core/STM32H750IB_sram.lds)

add_executable(
  ${BARELY_DAISY_TARGET}
  daisy.cpp
)
set_target_properties(
  ${BARELY_DAISY_TARGET} PROPERTIES
  SUFFIX ".elf"
  LINK_DEPENDS ${BARELY_DAISY_LINKER_SCRIPT}
)
target_compile_options(
  ${BARELY_DAISY_TARGET} PUBLIC
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
  ${BARELY_DAISY_TARGET} PUBLIC
  barelymusician
  barely_midi
  barely_oscillator
  barely_synth_instrument
  daisy
)
target_link_options(
  ${BARELY_DAISY_TARGET} PUBLIC
  -T ${BARELY_DAISY_LINKER_SCRIPT}
  -Wl,-Map=${BARELY_DAISY_TARGET}.map,--cref
  -Wl,--check-sections
  -Wl,--unresolved-symbols=report-all
  -Wl,--warn-common
  -Wl,--warn-section-align
  -Wl,--print-memory-usage
)

add_custom_command(
  TARGET ${BARELY_DAISY_TARGET} POST_BUILD
  COMMAND ${CMAKE_OBJCOPY}
  ARGS -O ihex
  -S ${BARELY_DAISY_TARGET}.elf
  ${BARELY_DAISY_TARGET}.hex
  BYPRODUCTS
  ${BARELY_DAISY_TARGET}.hex
  COMMENT "Generating HEX image"
  VERBATIM
)
add_custom_command(
  TARGET ${BARELY_DAISY_TARGET} POST_BUILD
  COMMAND ${CMAKE_OBJCOPY}
  ARGS -O binary
  -S ${BARELY_DAISY_TARGET}.elf
  ${BARELY_DAISY_TARGET}.bin
  BYPRODUCTS
  ${BARELY_DAISY_TARGET}.bin
  COMMENT "Generating binary image"
  VERBATIM
)

add_custom_target(
  ${BARELY_DAISY_TARGET}_program_dfu
  COMMAND dfu-util -a 0 -s 0x90040000:leave -D ${BARELY_DAISY_TARGET}.bin -d ,0483:df11
)

#!/bin/bash
#
# Builds and runs the BarelyMusician program for Dasiy.
#
# Options:
#   -d=|--daisy_toolchain_prefix=[value], default: "C:/Program Files/DaisyToolchain"

readonly DAISY_TOOLCHAIN_FILE="_deps/libdaisy-src/cmake/toolchains/stm32h750xx.cmake"

readonly GREEN="\e[32m"
readonly RESET="\e[0m"

parse_flags() {
  DAISY_TOOLCHAIN_PREFIX="C:/Program Files/DaisyToolchain"

  for i in "$@"; do
    case $i in
    -d=* | --daisy_toolchain_prefix=*)
      # toolchain prefix.
      DAISY_TOOLCHAIN_PREFIX="${i#*=}"
      shift
      ;;
    *)
      # unknown option.
      echo "Unknown option: ${i}" >&2
      exit 1
      ;;
    esac
  done

  readonly DAISY_TOOLCHAIN_PREFIX
}

main() {
  set -e

  parse_flags "$@"

  echo "Building BarelyMusician for Daisy..."
  mkdir -p cmake-build
  cd cmake-build
  cmake -DENABLE_UNITY=OFF -DENABLE_EXAMPLES=OFF -DENABLE_TESTS=OFF -DENABLE_DAISY=ON \
    -S ../ \
    -D CMAKE_TOOLCHAIN_FILE="${DAISY_TOOLCHAIN_FILE}" \
    -D TOOLCHAIN_PREFIX="${DAISY_TOOLCHAIN_PREFIX}" \
    -G "Unix Makefiles"
  make

  echo "Programming BarelyMusician on Daisy bootloader. Press any key to continue..."
  read -s -n 1
  make barely_daisy_program_dfu

  echo -e "${GREEN}Done!${RESET}"
}

main "$@"

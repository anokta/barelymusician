#!/bin/bash
#
# Builds BarelyMusician Unity plugins and copies them to the project plugins
# folder.
#
# Options:
#   -c=|--compilation_mode=[dbg|opt], default: opt

readonly WORKSPACE="$(bazel info workspace)"
readonly BAZEL_BIN="${WORKSPACE}/bazel-bin"

readonly NATIVE_BIN_DIR="${BAZEL_BIN}/unity/native"

readonly PLUGINS_DIR="${WORKSPACE}/platforms/unity/project/Assets/BarelyMusician/Plugins"
readonly PLUGINS_X64_DIR="${PLUGINS_DIR}/x86_64"

readonly PLUGIN_NAME="barelymusicianunity"

readonly LINUX_BIN_SRC_PATH="${NATIVE_BIN_DIR}/libunity_linux.so"
readonly LINUX_BIN_DST_PATH="${PLUGINS_X64_DIR}/lib${PLUGIN_NAME}.so"

readonly OSX_BIN_SRC_PATH="${NATIVE_BIN_DIR}/unity_osx.dylib"
readonly OSX_BIN_DST_PATH="${PLUGINS_X64_DIR}/${PLUGIN_NAME}.bundle"

readonly WINDOWS_BIN_SRC_PATH="${NATIVE_BIN_DIR}/unity_windows.dll"
readonly WINDOWS_BIN_DST_PATH="${PLUGINS_X64_DIR}/${PLUGIN_NAME}.dll"

parse_flags() {
  COMPILATION_MODE="opt"

  for i in "$@";
    do
      case $i in
        -c=*|--compilation_mode=*)
          # compilation mode.
          COMPILATION_MODE="${i#*=}"
          shift
          ;;
        *)
          # unknown option.
          echo "Unknown option: ${i}" >&2
          exit 1
          ;;
      esac
    done  
  
  readonly COMPILATION_MODE
}

main() {
  set -e
  
  parse_flags "$@"

  echo "Building BarelyMusician Unity plugins..."
  bazel build -c "${COMPILATION_MODE}" "//platforms/unity/native:all"

  if [[ -f "${LINUX_BIN_SRC_PATH}" ]]; then
    echo "Copying native Linux plugin into Unity project..."
    cp -rf "${LINUX_BIN_SRC_PATH}" "${LINUX_BIN_DST_PATH}"
  fi

  if [[ -f "${OSX_BIN_SRC_PATH}" ]]; then
    echo "Copying native OSX plugin into Unity project..."
    cp -rf "${OSX_BIN_SRC_PATH}" "${OSX_BIN_DST_PATH}"
  fi

  if [[ -f "${WINDOWS_BIN_SRC_PATH}" ]]; then
    echo "Copying native Windows plugin into Unity project..."
    cp -rf "${WINDOWS_BIN_SRC_PATH}" "${WINDOWS_BIN_DST_PATH}"
  fi
}

main "$@"

#!/bin/bash
#
# Builds BarelyMusician Unity plugins and copies them to the plugins folder.
#
# Options:
#   -c=|--compilation_mode=[dbg|opt], default: opt

readonly WORKSPACE="$(bazel info workspace)"
readonly BAZEL_BIN="${WORKSPACE}/bazel-bin"

readonly NATIVE_BIN_DIR="${BAZEL_BIN}/unity"

readonly PLUGINS_DIR="${WORKSPACE}/unity/Assets/BarelyMusician/Plugins"
readonly PLUGINS_ANDROID_DIR="${PLUGINS_DIR}/Android/libs"
readonly PLUGINS_X64_DIR="${PLUGINS_DIR}/x86_64"

readonly PLUGIN_NAME="barelymusicianunity"

readonly ANDROID_BIN_SRC_PATH="${NATIVE_BIN_DIR}/libunity_android.so"
readonly ANDROID_ARM32_BIN_DST_PATH="${PLUGINS_ANDROID_DIR}/armeabi-v7a/lib${PLUGIN_NAME}.so"
readonly ANDROID_ARM64_BIN_DST_PATH="${PLUGINS_ANDROID_DIR}/arm64-v8a/lib${PLUGIN_NAME}.so"

readonly LINUX_BIN_SRC_PATH="${NATIVE_BIN_DIR}/libunity_linux.so"
readonly LINUX_BIN_DST_PATH="${PLUGINS_X64_DIR}/lib${PLUGIN_NAME}.so"

readonly OSX_BIN_SRC_PATH="${NATIVE_BIN_DIR}/unity_osx.dylib"
readonly OSX_BIN_DST_PATH="${PLUGINS_X64_DIR}/${PLUGIN_NAME}.bundle"

readonly WINDOWS_BIN_SRC_PATH="${NATIVE_BIN_DIR}/unity_windows.dll"
readonly WINDOWS_BIN_DST_PATH="${PLUGINS_X64_DIR}/${PLUGIN_NAME}.dll"

readonly GREEN="\e[32m"

parse_flags() {
  COMPILATION_MODE="opt"

  for i in "$@"; do
    case $i in
    -c=* | --compilation_mode=*)
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

  case "$(uname -s)" in
  Linux)
    echo "Building BarelyMusician Unity native Linux plugin..."
    bazel build -c "${COMPILATION_MODE}" "//unity:libunity_linux.so"
    echo "Copying native Linux plugin into Unity project..."
    cp -rf "${LINUX_BIN_SRC_PATH}" "${LINUX_BIN_DST_PATH}"
    ;;

  Darwin)
    echo "Building BarelyMusician Unity native OSX plugin..."
    bazel build -c "${COMPILATION_MODE}" "//unity:unity_osx.dylib"
    echo "Copying native OSX plugin into Unity project..."
    cp -rf "${OSX_BIN_SRC_PATH}" "${OSX_BIN_DST_PATH}"
    ;;

  CYGWIN* | MINGW* | MSYS*)
    export MSYS2_ARG_CONV_EXCL="*"
    echo "Building BarelyMusician Unity native Windows plugin..."
    bazel build -c "${COMPILATION_MODE}" "//unity:unity_windows.dll"
    echo "Copying native Windows plugin into Unity project..."
    cp -rf "${WINDOWS_BIN_SRC_PATH}" "${WINDOWS_BIN_DST_PATH}"
    ;;

  *) ;;
  esac

  echo "Building BarelyMusician Unity native Android-ARM32 plugin..."
  bazel build -c "${COMPILATION_MODE}" --noenable_platform_specific_config \
    --config=android_armeabi-v7a "//unity:libunity_android.so"
    echo "Copying native Android-ARM32 plugin into Unity project..."
    cp -rf "${ANDROID_BIN_SRC_PATH}" "${ANDROID_ARM32_BIN_DST_PATH}"

  echo "Building BarelyMusician Unity native Android-ARM64 plugin..."
  bazel build -c "${COMPILATION_MODE}" --noenable_platform_specific_config \
    --config=android_arm64-v8a "//unity:libunity_android.so"
    echo "Copying native Android-ARM64 plugin into Unity project..."
    cp -rf "${ANDROID_BIN_SRC_PATH}" "${ANDROID_ARM64_BIN_DST_PATH}"

  echo -e "${GREEN}Done!"
}

main "$@"

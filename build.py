import argparse
import os
import subprocess
import sys

# TODO(#134): Add these to parsed arguments.
ANDROID_ABI = "arm64-v8a"
ANDROID_NATIVE_API_LEVEL = "android-21"
ANDROID_NDK_HOME = os.environ.get("ANDROID_NDK_HOME", "C:/Microsoft/AndroidSDK/ndk/27.0.12077973")
ANDROID_TOOLCHAIN_FILE = os.path.join(ANDROID_NDK_HOME, "build/cmake/android.toolchain.cmake")


DAISY_TOOLCHAIN_FILE = "_deps/libdaisy-src/cmake/toolchains/stm32h750xx.cmake"
DAISY_TOOLCHAIN_PREFIX = os.environ.get("DAISY_TOOLCHAIN_PREFIX", "C:/Program Files/DaisyToolchain")


def get_current_platform():
    if sys.platform.startswith("linux"):
        return ["linux"]
    elif sys.platform.startswith("darwin"):
        return ["mac"]
    elif sys.platform.startswith("win"):
        return ["windows"]
    return []


def parse_args():
    parser = argparse.ArgumentParser(
        description="build the barelymusician library",
        allow_abbrev=False,
    )
    parser.add_argument(
        "-p",
        "--platforms",
        choices=[
            "android",
            "daisy",
            "linux",
            "mac",
            "windows",
        ],
        nargs="+",
        default=get_current_platform(),
        help="specify the target platforms (defaults to the current platform)",
    )
    parser.add_argument(
        "-c",
        "--config",
        choices=[
            "debug",
            "release",
        ],
        default="release",
        help="specify the target build configuration (defaults to the release configuration)",
    )
    parser.add_argument(
        "-examples",
        action="store_true",
        help="build the examples for the selected platforms",
    )
    parser.add_argument(
        "-test",
        action="store_true",
        help="build unit tests for the selected platforms",
    )
    parser.add_argument(
        "-unity",
        action="store_true",
        help="build the unity native plugins for the selected platforms",
    )
    return parser.parse_args()


def run_command(command, cwd):
    print(command)
    subprocess.run(command, check=True, cwd=cwd, shell=True)


def build_platform(config, source_dir, build_dir, cmake_options):
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    cmake_generate_command = f'cmake -S "{source_dir}" -B "{build_dir}" {" ".join(cmake_options)}'
    run_command(cmake_generate_command, build_dir)

    cmake_build_command = f'cmake --build "{build_dir}" --config {config}'
    run_command(cmake_build_command, build_dir)


def build(args, source_dir, build_dir):
    print("Building the selected target platforms...")

    config = str.capitalize(args.config)
    platforms = args.platforms

    common_cmake_options = []
    if args.test:
        common_cmake_options.append("-DENABLE_TESTS=ON -DGTEST_COLOR=1")
    if args.examples:
        common_cmake_options.append("-DENABLE_EXAMPLES=ON")
    if args.unity:
        common_cmake_options.append("-DENABLE_UNITY=ON")

    if "android" in platforms:
        print("Building the Android targets...")
        print(f'"{ANDROID_TOOLCHAIN_FILE}"')
        android_build_dir = os.path.join(build_dir, "Android", ANDROID_ABI)
        android_cmake_options = [
            '-G "Ninja"',
            f'-DCMAKE_TOOLCHAIN_FILE="{ANDROID_TOOLCHAIN_FILE}"',
            f'-DANDROID_ABI="{ANDROID_ABI}"',
            f'-DANDROID_NDK="{ANDROID_NDK_HOME}"',
            f'-DANDROID_PLATFORM="{ANDROID_NATIVE_API_LEVEL}"',
            f'-DCMAKE_BUILD_TYPE="{config}"',
        ] + common_cmake_options
        build_platform(config, source_dir, android_build_dir, android_cmake_options)

    if "daisy" in platforms:
        print("Building the Daisy targets...")
        daisy_build_dir = os.path.join(build_dir, "Daisy")
        daisy_cmake_options = [
            '-G "Unix Makefiles"',
            f'-DCMAKE_TOOLCHAIN_FILE="{DAISY_TOOLCHAIN_FILE}"',
            f'-DTOOLCHAIN_PREFIX="{DAISY_TOOLCHAIN_PREFIX}"',
            "-DENABLE_DAISY=ON",  # ignore `common_cmake_options`
        ]
        build_platform(config, source_dir, daisy_build_dir, daisy_cmake_options)

    if "linux" in platforms:
        print("Building the Linux targets...")
        linux_build_dir = os.path.join(build_dir, "Linux")
        linux_cmake_options = [
            '-G "Unix Makefiles"',
            f'-DCMAKE_BUILD_TYPE="{config}"',
        ] + common_cmake_options
        build_platform(config, source_dir, linux_build_dir, linux_cmake_options)

    if "mac" in platforms:
        print("Building the Windows targets...")
        mac_build_dir = os.path.join(build_dir, "Mac")
        mac_cmake_options = ['-G "Xcode"'] + common_cmake_options
        build_platform(config, source_dir, mac_build_dir, mac_cmake_options)

    if "windows" in platforms:
        print("Building the Windows targets...")
        windows_build_dir = os.path.join(build_dir, "Windows")
        windows_cmake_options = ['-G "Visual Studio 17 2022"'] + common_cmake_options
        build_platform(config, source_dir, windows_build_dir, windows_cmake_options)


def package_unity(build_dir, unity_dir):
    print("Packaging the Unity native plugin artifacts...")


def run_tests(platforms, build_dir):
    ctest_command = "ctest --output-on-failure"
    for platform in platforms:
        run_command(ctest_command, os.path.join(build_dir, str.capitalize(platform)))


def main():
    args = parse_args()
    root_dir = os.path.abspath(os.path.dirname(__file__))

    build_dir = os.path.join(root_dir, "build")
    build(args, root_dir, build_dir)

    if args.unity:
        unity_dir = os.path.join(root_dir, "platforms", "unity")
        package_unity(build_dir, unity_dir)

    if args.test:
        run_tests(args.platforms, build_dir)


if __name__ == "__main__":
    main()

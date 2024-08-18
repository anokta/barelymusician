import argparse
import errno
import os
import shutil
import stat
import subprocess
import sys

ANDROID_NDK_HOME = os.environ.get("ANDROID_NDK_HOME", "C:/Microsoft/AndroidSDK/ndk/27.0.12077973")
ANDROID_TOOLCHAIN_FILE = os.path.join(ANDROID_NDK_HOME, "build/cmake/android.toolchain.cmake")

DAISY_TOOLCHAIN_FILE = "_deps/libdaisy-src/cmake/toolchains/stm32h750xx.cmake"


def parse_args():
    parser = argparse.ArgumentParser(
        description="build the barelymusician library",
        allow_abbrev=False,
    )
    parser.add_argument(
        "-c",
        "--config",
        choices=["debug", "release"],
        default="release",
        help="specify the build configuration (defaults to release)",
    )
    parser.add_argument(
        "--linux",
        action="store_true",
        default=sys.platform.startswith("linux"),
        help="build the linux targets (defaults to true on linux)",
    )
    parser.add_argument(
        "--mac",
        action="store_true",
        default=sys.platform.startswith("darwin"),
        help="build the mac targets (defaults to true on mac)",
    )
    parser.add_argument(
        "--windows",
        action="store_true",
        default=sys.platform.startswith("win"),
        help="build the windows targets (defaults to true on windows)",
    )
    parser.add_argument(
        "--android",
        action="store_true",
        help="build the android targets (requires android_abis and android_min_api to be valid)",
    )
    parser.add_argument(
        "--android_abis",
        choices=["armeabi-v7a", "arm64-v8a", "x86", "x86_64"],
        nargs="+",
        default=["armeabi-v7a", "arm64-v8a", "x86", "x86_64"],
        help="specify the abis for the android targets (defaults to all)",
    )
    parser.add_argument(
        "--android_min_api",
        type=int,
        default=21,
        help="specify the minimum api level for the android targets (defaults to 21)",
    )
    parser.add_argument(
        "--daisy",
        action="store_true",
        help="build and run the daisy program (requires daisy_toolchain_prefix to be valid)",
    )
    parser.add_argument(
        "--daisy_toolchain_prefix",
        default="C:/Program Files/DaisyToolchain",
        help="specify the daisy toolchain prefix for the daisy program",
    )
    parser.add_argument(
        "--examples",
        action="store_true",
        help="build the examples for the selected platforms",
    )
    parser.add_argument(
        "--run_demo",
        default=None,
        help="specify the examples demo to run (defaults to none)",
    )
    parser.add_argument(
        "--test",
        action="store_true",
        help="build and run the unit tests for the selected platforms",
    )
    parser.add_argument(
        "--unity",
        action="store_true",
        help="build the unity native plugins for the selected platforms",
    )
    parser.add_argument(
        "--clean",
        action="store_true",
        help="clean the previous build before the new build",
    )
    parser.add_argument(
        "--skip_generate",
        action="store_true",
        help="skip the cmake generate step",
    )
    parser.add_argument(
        "--skip_build",
        action="store_true",
        help="skip the cmake build step",
    )
    return parser.parse_args()


def clean(build_dir):
    print("Cleaning the previous build...")

    def onerror(func, path, excinfo):
        if func in (os.unlink, os.rmdir) and excinfo[1].errno == errno.EACCES:
            os.chmod(path, stat.S_IWRITE)
            func(path)

    shutil.rmtree(build_dir, onerror=onerror)


def run_command(command, cwd):
    print(command)
    subprocess.run(command, check=True, cwd=cwd, shell=True)


def build_platform(args, config, source_dir, build_dir, cmake_options):
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    if not args.skip_generate:
        generate_command = f'cmake -S "{source_dir}" -B "{build_dir}" {" ".join(cmake_options)}'
        run_command(generate_command, build_dir)

    if not args.skip_build:
        build_command = f'cmake --build "{build_dir}" --config {config}'
        if args.run_demo:
            build_command += f" --target barelymusician_examples_{args.run_demo}"
        run_command(build_command, build_dir)


def build(args, source_dir, build_dir):
    if args.clean and os.path.exists(build_dir):
        clean(build_dir)

    if args.skip_generate and args.skip_build:
        return

    config = str.capitalize(args.config)

    if args.daisy:
        daisy_build_dir = os.path.join(build_dir, "Daisy")
        daisy_cmake_options = [
            '-G "Unix Makefiles"',
            f'-DCMAKE_TOOLCHAIN_FILE="{DAISY_TOOLCHAIN_FILE}"',
            f'-DTOOLCHAIN_PREFIX="{args.daisy_toolchain_prefix}"',
            "-DENABLE_DAISY=ON",
        ]
        build_platform(args, config, source_dir, daisy_build_dir, daisy_cmake_options)

    common_cmake_options = []
    if args.unity:
        common_cmake_options.append("-DENABLE_UNITY=ON")

    if args.android:
        print("Building the Android targets...")
        for android_abi in args.android_abis:
            android_build_dir = os.path.join(build_dir, "Android", android_abi)
            android_cmake_options = [
                '-G "Ninja"',
                f'-DCMAKE_TOOLCHAIN_FILE="{ANDROID_TOOLCHAIN_FILE}"',
                f'-DANDROID_ABI="{android_abi}"',
                f'-DANDROID_NDK="{ANDROID_NDK_HOME}"',
                f'-DANDROID_PLATFORM="android-{args.android_min_api}"',
                f'-DCMAKE_BUILD_TYPE="{config}"',
            ] + common_cmake_options
            build_platform(args, config, source_dir, android_build_dir, android_cmake_options)

    if args.test:
        common_cmake_options.append("-DENABLE_TESTS=ON -DGTEST_COLOR=1")
    if args.examples or args.run_demo:
        common_cmake_options.append("-DENABLE_EXAMPLES=ON")

    if args.linux:
        print("Building the Linux targets...")
        linux_build_dir = os.path.join(build_dir, "Linux")
        linux_cmake_options = [
            '-G "Unix Makefiles"',
            f'-DCMAKE_BUILD_TYPE="{config}"',
        ] + common_cmake_options
        build_platform(args, config, source_dir, linux_build_dir, linux_cmake_options)

    if args.mac:
        print("Building the Windows targets...")
        mac_build_dir = os.path.join(build_dir, "Mac")
        mac_cmake_options = ['-G "Xcode"'] + common_cmake_options
        build_platform(args, config, source_dir, mac_build_dir, mac_cmake_options)

    if args.windows:
        print("Building the Windows targets...")
        windows_build_dir = os.path.join(build_dir, "Windows")
        windows_cmake_options = ['-G "Visual Studio 17 2022"'] + common_cmake_options
        build_platform(args, config, source_dir, windows_build_dir, windows_cmake_options)


def run_daisy_program(build_dir):
    input("Running the Daisy program, press enter to continue...")

    daisy_build_dir = os.path.join(build_dir, "Daisy")
    daisy_make_command = "make barelymusiciandaisy_program_dfu"
    run_command(daisy_make_command, daisy_build_dir)


def run_demo(args, build_dir):
    input("Running the demo, press enter to continue...")

    for platform in os.listdir(build_dir):
        if (
            (platform == "Linux" and sys.platform.startswith("linux"))
            or (platform == "Mac" and sys.platform.startswith("darwin"))
            or (platform == "Windows" and sys.platform.startswith("win"))
        ):
            demo_dir = f"{build_dir}/{platform}/bin/{str.capitalize(args.config)}"
            demo_path = os.path.join(demo_dir, args.run_demo)
            if platform == "Windows":
                demo_path += ".exe"
            run_command(demo_path, demo_dir)
            break


def run_tests(build_dir):
    ctest_command = "ctest --output-on-failure"
    for platform in os.listdir(build_dir):
        if (
            (platform == "Linux" and sys.platform.startswith("linux"))
            or (platform == "Mac" and sys.platform.startswith("darwin"))
            or (platform == "Windows" and sys.platform.startswith("win"))
        ):
            run_command(ctest_command, os.path.join(build_dir, platform))


def main():
    args = parse_args()
    root_dir = os.path.abspath(os.path.dirname(__file__))

    build_dir = os.path.join(root_dir, "build")
    build(args, root_dir, build_dir)

    if args.test:
        run_tests(build_dir)

    if args.daisy:
        run_daisy_program(build_dir)

    if args.run_demo is not None:
        run_demo(args, build_dir)


if __name__ == "__main__":
    main()

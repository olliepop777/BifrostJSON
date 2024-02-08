import os
import subprocess
import platform
import re
import argparse

# Basic Usage: python3 build.py
# Help: python3 build.py --help

BIFROST_JSON_VERSION = "1.0.0"

PLATFORM_INFO = {
    "Windows": {
        "bifrost_path": "C:\\Program Files\\Autodesk\\Bifrost\\",
        "nice_platform_name": "windows",
        "vswhere_path": "C:\\Program Files (x86)\\Microsoft Visual Studio\\Installer\\vswhere.exe",
        # Only 2019 is official supported at the time of writing,
        # but 2019 build tools can be installed in 2022 as well and they work
        "visual_studio_version_map": {
            "2022": "17",
            "2019": "16",
        }
    },
    "Darwin": {
        "bifrost_path": "/Applications/Autodesk/bifrost/",
        "nice_platform_name": "macOS"
    },
    "Linux": {
        "bifrost_path": "/usr/autodesk/bifrost/",
        "nice_platform_name": "linux"
    }
}
CMAKE_BUILD_DIR = "build_output"

def check_cmake_exists():
    try:
        subprocess.check_call(["cmake", "--version"])
        build_script_print("CMake found on system PATH")
    except Exception as e:
        raise Exception("There was an error finding CMake. Check that CMake is on the system PATH") from e

def get_bifrost_install_path(platform_name):
    base_bifrost_install_path = PLATFORM_INFO[platform_name]["bifrost_path"]
    max_maya_version = None
    max_bifrost_version = None
    
    if os.path.exists(base_bifrost_install_path):
        maya_versions = [d for d in os.listdir(base_bifrost_install_path) if os.path.isdir(os.path.join(base_bifrost_install_path, d))]
        max_maya_version = max(maya_versions)

    if max_maya_version:
        maya_version_path = os.path.join(base_bifrost_install_path, max_maya_version)
        bifrost_versions = [d for d in os.listdir(maya_version_path) if os.path.isdir(os.path.join(maya_version_path, d))]
        max_bifrost_version = max(bifrost_versions)

    if not max_bifrost_version:
        raise FileNotFoundError(
            ("Bifrost installation not found at the default path for your platform: {}\n"
            "Please provide it manually using the --bifrost-path option.")
            .format(base_bifrost_install_path)
        )

    return os.path.join(maya_version_path, max_bifrost_version, "bifrost")

def build_with_cmake(bifrost_install_path, platform_name, is_release):
    build_args = [
        "cmake", "-S", ".",
        "-B", CMAKE_BUILD_DIR,
        "-DBIFROST_LOCATION={}".format(bifrost_install_path),
        "-DBIFROST_JSON_RELEASE_VERSION={}".format(BIFROST_JSON_VERSION),
        "-DCMAKE_BUILD_TYPE=Release"
    ]

    if platform_name == "Darwin":
        build_args.append(get_macOS_architecture(is_release))

    if platform_name == "Windows":
        build_args += set_windows_compiler()

    release_name = generate_release_name(platform_name, bifrost_install_path)
    build_args.append("-DCMAKE_INSTALL_PREFIX={}".format(release_name))

    subprocess.check_call(build_args)

    # Build the operator
    subprocess.check_call(
        [
            "cmake", "--build", CMAKE_BUILD_DIR,
            "--config", "Release",
            "--target", "install"
        ]
    )

def generate_release_name(platform_name, bifrost_install_path):
    success = True
    try:
        bif_install_version = os.path.split(bifrost_install_path)[0]
        bif_install_version = os.path.split(bif_install_version)[1]
        # Matches a version string like "2.7.1.1"
        pattern = re.compile(r'^\d+(\.\d+){3}$')
        is_valid_bif_path_version = bool(pattern.match(bif_install_version))
        success = success and is_valid_bif_path_version
    except:
        success = False
    if not success:
        raise Exception(
            "Bifrost version could not be extracted from bifrost location: {}"
            .format(bifrost_install_path)
        )

    return "BifrostJSON_v{}_{}_bif{}".format(
        BIFROST_JSON_VERSION,
        PLATFORM_INFO[platform_name]["nice_platform_name"],
        bif_install_version
    )


def set_windows_compiler():
    vswhere_path = PLATFORM_INFO["Windows"]["vswhere_path"]
    try:
        vs_version_year = subprocess.check_output([
            vswhere_path, "-latest", "-property",
            "catalog_productLineVersion"
        ]).decode().rstrip("\r\n")
    except Exception as e:
        raise Exception(
            ("No valid Visual Studio Install found with standard vswhere.exe path:\n"
             "{}".format(vswhere_path))
        ) from e
    build_script_print("Visual Studio version is: {}".format(vs_version_year))

    vs_version_num = PLATFORM_INFO["Windows"]["visual_studio_version_map"][vs_version_year]
    build_args = ["-G Visual Studio {} {}".format(vs_version_num, vs_version_year)]
    if vs_version_year >= "2022":
        # v.142 is Visual Studio 2019 Build tools. Must be installed manually in Visual Studio 2022
        build_script_print(
            (
                "Visual Studio 2022 or higher detected. Please make sure to manually install the 2019 build tools\n" 
                "inside of Visual Studio 2022 (or higher), as only the 2019 tools are compatible with Bifrost SDK\n"
             )
        )
        build_args += ["-T v142"]
    return build_args

def get_macOS_architecture(is_release):
    build_flag = "-DCMAKE_OSX_ARCHITECTURES="
    if is_release:
        # Build a universal binary for public releases
        return build_flag + "arm64;x86_64"
    
    # Just build based off the system architecture in normal builds
    arch = platform.processor()
    if arch == "arm":
        return build_flag + "arm64"
    else:
        return build_flag + "x86_64"

def build_script_print(print_msg):
    print("PYTHON_BIFROST_BUILD_SCRIPT - " + print_msg)

def main(args):
    is_release = args.release
    bifrost_install_path = args.bifrost_path

    check_cmake_exists()
    platform_name = platform.system()
    if not bifrost_install_path:
        bifrost_install_path = get_bifrost_install_path(platform_name)
        build_script_print("Detected Bifrost install path: {}".format(bifrost_install_path))

    build_with_cmake(bifrost_install_path, platform_name, is_release)

def setup_args():
    parser = argparse.ArgumentParser(description="Build the BifrostJSON operators and helpers")
    parser.add_argument("-r", "--release", action="store_true", help="If specified, builds for public release")
    parser.add_argument("-bp", "--bifrost-path", help="Specify an alternate path for the Bifrost install")
    args = parser.parse_args()
    return args

if __name__ == "__main__":
    args = setup_args()
    main(args)

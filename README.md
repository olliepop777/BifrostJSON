# BifrostJSON

## <span style="color:red">This repository has been archived</span>

**BifrostJSON has moved to [BifrostFileIO](https://github.com/olliepop777/BifrostFileIO)**

<hr/>

BifrostJSON is a library for interfacing with JSON data inside the Bifrost graph visual programming interface for Autodesk Maya. It allows for translating JSON data into the corresponding Bifrost types.

Usage video: https://youtu.be/xj2GzIATwXg

## Features
* Read JSON data from a file on disk or an Amino String input
* Helper nodes to get and set data inside the Bifrost graph via expression, instead of chaining together many get/set nodes in arrays/Objects
* Printing input data to console for debugging
* Output data as a JSON string for consumption in Maya

## Installation
* Head to the [Releases page](https://github.com/olliepop777/BifrostJSON/releases) and download the version built for your platform (Windows, Linux, etc.) and Bifrost version.
* Edit your `BIFROST_LIB_CONFIG_FILES` environment variable to point to the configuration file located in `<BifrostJSON_vX.X.X_PLATFORM_bifX.X.X.X>/BifrostJSON-<version_number>/BifrostJSONPackConfig.json`.

  For example, on MacOS: 
  ```
  BIFROST_LIB_CONFIG_FILES = /path/to/BifrostJSON_v1.0.0_PLATFORM_bif2.7.1.1/BifrostJSON-1.0.0/BifrostJSONPackConfig.json
  ```

  Make sure you add the right delimiter for your platform if adding multiple configuration files, i.e `:` (colon), `;` (semi-colon), etc.

* Note that you must use the `BIFROST_LIB_CONFIG_FILES` ENV to install the operator. For compiled operators this is required, unlike other compounds where it is enough to drop files into the Autodesk directory located in the platform's home directory

## Usage

### Reading JSON
Use the core `read_json` node to read JSON. Input JSON can either come from a file, via the `json_file` input, or a JSON string via the `json_str` input. By default a Bifrost Object is outputted. Optionally, a JSON string can be outputted as well via the `output_a_json_str` port. Consult the node documentation in the Bifrost Graph Editor for more detailed explanations.

### Cached JSON Reading
Since Bifrost is evaluating on every frame you may not want to read from a JSON file repeatedly if the file is not changing very often. To save filesystem resources, use the `read_json_cached` node. The file can be `read_once` or `read_on_frame`. They can be used in combination with each other.

### Debug
If the debug option is checked, the parsed JSON will be printed to the console (not the same as Maya's Script Editor). This is not the final Bifrost translation, it is just for checking the input JSON data. More verbose debug information like what types are being converted and why arrays are being cast to `array<any>` could be a possible future feature.

### Array Types
Since arrays can contain mixed types in JSON data, and Bifrost is more strongly typed, arrays may or may not be cast to the type `array<any>`. For example, if an array only holds integer types, then it will be translated to the Bifrost type `array<long>`. Arrays up to 3 levels deep can be translated this way, unless one of the arrays contains mixed types. Arrays of higher dimension than 3D arrays will fall back to nested `array<any>`. See the `read_json` node documentation for full details.

### Retrieving Data
The helper node `get_with_access_expr` can be used to retrieve data (similar to getters and setters in other programming languages) with a single expression. Keys inside a curly brace `{}` will retrieve object keys, and indices inside a bracket `[]` will retrieve an array index. For example:

`{json_data}[9]{first_name}{age}`

would retrieve the key `json_data`, the 9th index in the array of people, the `first_name` key, and finally the `age` key. See the node `get_with_access_expr` documentation for full details.

### Setting Data
Similar to the `get_with_access_expr` node, the `set_with_access_expr` node will set the data at the specified expression, and return the modified data in the `out_obj_or_array` port.

## Building the operator
You may wish to build the operator for newer/older versions of Maya/Bifrost that are not provided. The goal is to have pre-built releases for the latest Bifrost version, but they may not be ready promptly.

A build script called `build.py` is included which will search the default path for the different platforms and attempt to build for the highest Maya and Bifrost combination found on the system. Alternatively you can pass the path of the installation if you want to build for an older version, or if your installation is located at a non-default path for the platform. Run it from a shell with:

`python3 build.py`

OR

`python3 build.py --help`

for all usage options. If you have a non-default installation path for Bifrost, you must pass the path manually like so:

`python3 build.py --bifrost-path </path/to/bifrost/install>`

for example:

`python3 build.py -bp /my/cool/path/bifrost/maya2023/2.6.0.0/bifrost`

Below are more detailed instructions for each platform and the requirements for building.

## Building with CMake

You need CMake 3.17 and up: https://cmake.org/

The public Bifrost SDK was first released with Bifrost 2.5, therefore Bifrost 2.5 or higher is required to build this operator.

### Windows

1. Install CMake via the official installer or the Chocolatey package manager: `choco install cmake`
2. Install the full Visual Studio IDE or just the Build Tools (testing was done with the full IDE). Minimum required version is Visual Studio 2019. If using 2022 (or higher), only the 2019 toolchain is supported by the Bifrost SDK. Make sure to install the build tools v.142 (Visual Studio 2019) manually through the Visual Studio Installer. Once installed, the build script will automatically handle picking the right generator.

### MacOS

1. Install the XCode full IDE or alternatively, just the command line tools with: `xcode-select --install`
2. Install CMake via the official installer or the Homebrew package manager: `brew install cmake`
3. Both Apple Silicon (arm64) and Intel X86 (x86_64) architectures are supported on Bifrost 2.7 and higher, in combination with Maya 2024 and higher. Previous versions fall back to `x86_64`. Your processor's architecture will automatically be selected. To build a universal binary, pass the release flag to the build script: `python3 build.py --release`

### Linux

1. All testing was done on AlmaLinux
2. Install `gcc` and `g++` via your distro's package manager. On AlmaLinux:
    `sudo dnf install gcc g++`
3. Install `cmake` via your distro's package manager. On AlmaLinux: `sudo dnf install cmake`

# Third Paty Credits
Internally BifrostJSON uses the `nlohmann::json` library: https://github.com/nlohmann/json

The header is included in the project sources for convenient self compilation.

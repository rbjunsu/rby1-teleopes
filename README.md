# rby1-sdk

<p>
<a href="https://github.com/RainbowRobotics/rby1-sdk/actions">
<img src="https://img.shields.io/github/actions/workflow/status/RainbowRobotics/rby1-sdk/release.yml" alt="CI">
</a>
<a href="https://github.com/RainbowRobotics/rby1-sdk/issues">
<img src="https://img.shields.io/github/issues/RainbowRobotics/rby1-sdk" alt="Issues">
</a>
<a href="https://github.com/RainbowRobotics/rby1-sdk/releases">
<img src="https://img.shields.io/github/v/release/RainbowRobotics/rby1-sdk" alt="Releases">
</a>
<a href="https://github.com/RainbowRobotics/rby1-sdk/blob/main/LICENSE">
<img src="https://img.shields.io/github/license/RainbowRobotics/rby1-sdk" alt="Apache-2.0">
</a>
<a href="https://pypi.org/project/rby1-sdk/">
<img src="https://img.shields.io/pypi/pyversions/rby1-sdk" alt="python">
</a>
</p>

**rby1-sdk** is an SDK designed for seamless control and development with the RB-Y1 robot. It provides Python and C++
APIs,
making it easy to integrate into various applications, from basic scripting to advanced real-time control.

# Get Started Teleoperation Example


#### Example Descriptions

| Example     | Description                                                                                     |
|-------------|-------------------------------------------------------------------------------------------------|
| `big_aloha` | Stand-type configuration with an Aloha-style handle                                             |
| `big`       | Stand-type configuration with a basic handle                                                    |
| `exo`       | Wearable-type configuration with a basic handle                                                 |

---



## C++ Installation

### Build From Source

#### Step 1: Install Conan

[Conan](https://conan.io/) is required for dependency management:

```bash
pip install conan
```

#### Step 2: Clone the Repository

```bash
git clone --recurse-submodules git@github.com:rbjunsu/rby1-teleopes.git
```

#### Step 3: Install or Build Dependencies

```bash
conan install . -s build_type=Release -b missing -of build
```

> [!NOTE]
> On `aarch64` platforms (e.g., ARM64 Ubuntu runners), please make sure to set the appropriate compiler explicitly:
>
> ```bash
> export CC=/usr/bin/aarch64-linux-gnu-gcc
> ```

#### Step 4: Configure, Build, and Install

##### (Option 1) CMake >= 3.23

```bash
cmake --preset conan-release -D BUILD_EXAMPLES=ON
cmake --build --preset conan-release

# Install
cmake --build --preset conan-release --target install
```

##### (Option 2) CMake < 3.23

```bash
cd build
cmake .. -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=./conan_toolchain.cmake -DCMAKE_POLICY_DEFAULT_CMP0091=NEW -DCMAKE_BUILD_TYPE=Release
cmake --build . # or make

# Install
make install
```


## C++ Example

```c++
./build/examples/cpp/example_teleoperation_big_aloha <server address> <servo> <mode>

      # example
      # ./build/examples/cpp/example_teleoperation_big_aloha 192.168.30.1:50051 impedance
      
```

#### Option Descriptions

| Option             | Description                                                                 |
|--------------------|-----------------------------------------------------------------------------|
| `<server address>` | IP address of the Robot PC                                                  |
| `<servo>`          | Select the servo to turn on (default = all, except 'head')                  |
| `<mode>`           | Control mode: choose either `position` (default) or `impedance`             |


---


## ARM Intellisense Issue

For issues with ARM Intellisense, refer to the
following [GitHub Issue](https://github.com/microsoft/vscode-cpptools/issues/7413).

To fix the Intellisense problem, add the following code at the top of your source file:

```c++
// Add this at the top of your source file
#if __INTELLISENSE__
#undef __ARM_NEON
#undef __ARM_NEON__
#endif
```

## Resources and Support

- Documentation: [https://rainbowrobotics.github.io/rby1-dev/](https://rainbowrobotics.github.io/rby1-dev/)
- Official Website: [Rainbow Robotics](https://www.rainbowrobotics.com/rby1eng)
- Official Support Email: rby.support@rainbow-robotics.com
- GitHub Discussions: [Join the Discussion](https://github.com/RainbowRobotics/rby1-sdk/discussions)
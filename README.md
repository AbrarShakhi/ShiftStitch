# ShiftStitch

**ShiftStitch** is a lightweight C++ panorama stitching library based on a SIFT feature matching and SIFT-based alignment algorithm.

---

## Features

* Fast panorama stitching
* SIFT-style feature matching
* Simple API
* Cross-platform (Linux, macOS, Windows, Android*)
* Built on OpenCV

---

## Requirements

* CMake ≥ 3.16
* C++17 compatible compiler
* OpenCV

---

## Build Instructions

### Linux (Ubuntu)

#### 1. Install dependencies

```bash
sudo apt update
sudo apt install build-essential cmake
sudo apt install libopencv-dev
```

---

#### 2. Build the project

```bash
python3 scripts/build_linux.py
```

> This script configures and builds the project using CMake.

---

#### 3. Run the example

```bash
./build/example_basic

# LD_LIBRARY_PATH=. ./example_basic
```

---

### macOS

#### 1. Install dependencies

Using Homebrew:

```bash
brew install cmake opencv
```

---

#### 2. Build

```bash
python3 scripts/build_linux.py
```

---

#### 3. Run

```bash
./example_basic
```

---

### Windows

#### 1. Install dependencies

* Install Visual Studio (with C++ support)
* Install CMake
* Install OpenCV and set `OpenCV_DIR`

---

#### 2. Build

```bash
mkdir build
cd build
cmake .. -A x64
cmake --build . --config Release
```

---

#### 3. Run

Make sure `ShiftStitch.dll` and OpenCV DLLs are in the same folder:

```bash
example_basic.exe
```

---

### Android (Advanced)

* Provide `OpenCV_DIR` manually
* Use Android NDK + toolchain
* Build via CMake toolchain file

---

## CMake Options

| Option                       | Description          | Default |
| ---------------------------- | -------------------- | ------- |
| `SHIFTSTITCH_BUILD_TESTS`    | Build tests          | ON      |
| `SHIFTSTITCH_BUILD_EXAMPLES` | Build examples       | ON      |
| `SHIFTSTITCH_FETCH_OPENCV`   | Auto-download OpenCV | OFF     |

Example:

```bash
cmake -DSHIFTSTITCH_FETCH_OPENCV=ON ..
```

---

## Project Structure

```
ShiftStitch/
├── include/        # Public headers
├── src/            # Library source
├── examples/       # Example programs
├── tests/          # Unit tests
├── scripts/        # Build scripts
├── cmake/          # CMake helpers
```

---

## Linking in Your Project

After installation:

```cmake
find_package(ShiftStitch REQUIRED)

target_link_libraries(your_app ShiftStitch::ShiftStitch)
```

---

## Runtime Notes (Linux)

If you see:

```
error while loading shared libraries: libShiftStitch.so: cannot open shared object file
```

Run:

```bash
LD_LIBRARY_PATH=. ./example_basic
```

Or install the library system-wide:

```bash
sudo cp libShiftStitch.so /usr/local/lib/
sudo ldconfig
```

---

## Notes

* The library is built as a **shared library (`.so/.dll/.dylib`)**
* OpenCV is a required dependency
* Make sure runtime libraries are discoverable

---

## License

project is under [Apache License](./LICENSE)

---

## Contributing

Pull requests and issues are welcome!

---

## Author

- A1
- A2
- A3

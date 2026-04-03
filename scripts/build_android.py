import os
from common import run, ensure_build_dir

# REQUIRED ENV VARIABLES
NDK = os.environ.get("ANDROID_NDK_HOME")
OPENCV_ANDROID = os.environ.get("OpenCV_DIR")

if not NDK:
    raise RuntimeError("❌ ANDROID_NDK_HOME not set")

if not OPENCV_ANDROID:
    raise RuntimeError("❌ OpenCV_DIR not set (Android OpenCV SDK path)")

ABI = "arm64-v8a"   # change if needed
BUILD_DIR = ensure_build_dir(f"build-android-{ABI}")

toolchain = os.path.join(NDK, "build/cmake/android.toolchain.cmake")

cmake_cmd = [
    "cmake",
    "..",
    f"-DCMAKE_TOOLCHAIN_FILE={toolchain}",
    f"-DANDROID_ABI={ABI}",
    "-DANDROID_PLATFORM=android-24",
    "-DCMAKE_BUILD_TYPE=Release",
    f"-DOpenCV_DIR={OPENCV_ANDROID}",
]

run(cmake_cmd, cwd=BUILD_DIR)
run(["cmake", "--build", ".", "-j"], cwd=BUILD_DIR)

print(f"\n✅ Android build complete for {ABI}")
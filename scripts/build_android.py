import os
import argparse
from common import run, ensure_build_dir

parser = argparse.ArgumentParser(description="Build ShiftStitch (Android)")
parser.add_argument("-a", "--abi", default="arm64-v8a")
parser.add_argument("-t", "--type", default="Release")
parser.add_argument("-c", "--clean", action="store_true")

args = parser.parse_args()

NDK = os.environ.get("ANDROID_NDK_HOME")
OPENCV_ANDROID = os.environ.get("OpenCV_DIR")

if not NDK:
    raise RuntimeError("❌ ANDROID_NDK_HOME not set")

if not OPENCV_ANDROID:
    raise RuntimeError("❌ OpenCV_DIR not set (Android SDK path)")

BUILD_DIR = ensure_build_dir(f"build-android-{args.abi}", clean=args.clean)

toolchain = os.path.join(NDK, "build/cmake/android.toolchain.cmake")

cmake_cmd = [
    "cmake",
    "..",
    f"-DCMAKE_TOOLCHAIN_FILE={toolchain}",
    f"-DANDROID_ABI={args.abi}",
    "-DANDROID_PLATFORM=android-24",
    f"-DCMAKE_BUILD_TYPE={args.type}",
    f"-DOpenCV_DIR={OPENCV_ANDROID}",
]

run(cmake_cmd, cwd=BUILD_DIR)
run(["cmake", "--build", ".", "-j"], cwd=BUILD_DIR)

print(f"\n✅ Android build complete ({args.abi}, {args.type})")
import os
import argparse
from common import run, ensure_build_dir


parser = argparse.ArgumentParser(description="Build ShiftStitch (Windows)")
parser.add_argument("-i", "--install-opencv", action="store_true")
parser.add_argument("-t", "--type", default="Release")
parser.add_argument("-c", "--clean", action="store_true")

args = parser.parse_args()

BUILD_DIR = ensure_build_dir("build", clean=args.clean)

cmake_cmd = [
    "cmake",
    ".."
]

if os.environ.get("OpenCV_DIR"):
    cmake_cmd.append(f"-DOpenCV_DIR={os.environ['OpenCV_DIR']}")

if args.install_opencv:
    cmake_cmd.append("-DSHIFTSTITCH_FETCH_OPENCV=ON")

run(cmake_cmd, cwd=BUILD_DIR)

run([
    "cmake", "--build", ".", "--config", args.type
], cwd=BUILD_DIR)

print(f"\n✅ Windows build complete ({args.type})")
import os
from common import run, ensure_build_dir

BUILD_DIR = ensure_build_dir("build")

opencv_dir = os.environ.get("OpenCV_DIR")

cmake_cmd = [
    "cmake",
    "..",
    "-DCMAKE_BUILD_TYPE=Release"
]

if opencv_dir:
    cmake_cmd.append(f"-DOpenCV_DIR={opencv_dir}")

run(cmake_cmd, cwd=BUILD_DIR)

# Windows build (multi-config)
run(["cmake", "--build", ".", "--config", "Release"], cwd=BUILD_DIR)

print("\n✅ Build complete (Windows)")
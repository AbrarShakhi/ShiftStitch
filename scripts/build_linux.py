import os
from common import run, ensure_build_dir

BUILD_DIR = ensure_build_dir("build")

# Optional: set OpenCV_DIR if custom install
opencv_dir = os.environ.get("OpenCV_DIR")

cmake_cmd = [
    "cmake",
    "..",
    "-DCMAKE_BUILD_TYPE=Release"
]

if opencv_dir:
    cmake_cmd.append(f"-DOpenCV_DIR={opencv_dir}")

run(cmake_cmd, cwd=BUILD_DIR)
run(["cmake", "--build", ".", "-j"], cwd=BUILD_DIR)

print("\n✅ Build complete (Linux/macOS)")
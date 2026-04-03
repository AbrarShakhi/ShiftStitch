import os
import shutil
import subprocess
import sys


def run(cmd, cwd=None):
    print(f"\n>>> {' '.join(cmd)}\n")
    result = subprocess.run(cmd, cwd=cwd)
    if result.returncode != 0:
        print("❌ Command failed")
        sys.exit(result.returncode)


def ensure_build_dir(path="build", clean=False):
    if clean and os.path.exists(path):
        print(f"🧹 Cleaning build directory: {path}")
        shutil.rmtree(path)

    os.makedirs(path, exist_ok=True)
    return os.path.abspath(path)
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

#include "shiftstitch/shiftstitch.hpp"
#include "shiftstitch/sift.hpp"

namespace fs = std::filesystem;
using namespace shiftstitch;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <image_directory>\n";
        return EXIT_FAILURE;
    }

    std::string dirPath = argv[1];
    std::vector<std::string> imagePaths;

    // Read all files from directory
    try {
        for (const auto& entry : fs::directory_iterator(dirPath)) {
            if (entry.is_regular_file()) {
                std::string path = entry.path().string();

                // Optional: filter by image extensions
                if (entry.path().extension() == ".jpg" ||
                    entry.path().extension() == ".jpeg" ||
                    entry.path().extension() == ".png") {
                    imagePaths.push_back(path);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error reading directory: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    if (imagePaths.empty()) {
        std::cerr << "No images found in directory.\n";
        return EXIT_FAILURE;
    }

    // Optional: sort paths for consistent stitching order
    std::sort(imagePaths.begin(), imagePaths.end());

    auto stitcherResult = ShiftStitcher::create(imagePaths);
    if (stitcherResult.isErr()) {
        std::cerr << stitcherResult.error().toString() << '\n';
        return EXIT_FAILURE;
    }

    ShiftStitcher& stitcher = stitcherResult.value();

    SIFT siftAlgorithm;
    auto panoramaResult = stitcher.createPanorama(siftAlgorithm);
    if (panoramaResult.isErr()) {
        std::cerr << panoramaResult.error().toString() << '\n';
        return EXIT_FAILURE;
    }

    auto saveResult = stitcher.savePanorama("output.jpg");
    if (saveResult.isErr()) {
        std::cerr << saveResult.error().toString() << '\n';
        return EXIT_FAILURE;
    }

    std::cout << "Panorama saved successfully.\n";
    return EXIT_SUCCESS;
}

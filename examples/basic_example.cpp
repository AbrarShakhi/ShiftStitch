#include "shiftstitch/shiftstitch.hpp"
#include "shiftstitch/sift.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using namespace shiftstitch;

int main(int argc, char* argv[]) {
	if (argc < 4) {
		std::cerr << "Usage: " << argv[0] << " <image1> <image2> [image3 ...] <output>\n";
		return EXIT_FAILURE;
	}

	std::vector<std::string> image_list;

	// All except last argument are input images
	for (int i = 1; i < argc - 1; ++i)
		image_list.push_back(argv[i]);

	// Last argument is output file
	std::string output_file = argv[argc - 1];

	auto stitcher_result = ShiftStitcher::create(image_list);
	if (stitcher_result.isErr()) {
		std::cerr << stitcher_result.error().toString() << '\n';
		return EXIT_FAILURE;
	}

	ShiftStitcher& stitcher = stitcher_result.value();

	SIFT sift_algorithm;
	auto panorama_result = stitcher.createPanorama(sift_algorithm);
	if (panorama_result.isErr()) {
		std::cerr << panorama_result.error().toString() << '\n';
		return EXIT_FAILURE;
	}

	auto save_result = stitcher.savePanorama(output_file);
	if (save_result.isErr()) {
		std::cerr << save_result.error().toString() << '\n';
		return EXIT_FAILURE;
	}

	std::cout << "Panorama saved successfully to " << output_file << ".\n";
	return EXIT_SUCCESS;
}

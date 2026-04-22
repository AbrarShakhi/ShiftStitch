#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "shiftstitch/shiftstitch.hpp"
#include "shiftstitch/sift.hpp"

using namespace shiftstitch;

int main(int argc, char* argv[]) {
	if (argc < 3) {
		std::cerr << "Usage: " << argv[0] << " <image1> <image2> image3 ...\n";
		return EXIT_FAILURE;
	}
	std::vector<std::string> image_list;
	for (int i = 1; i < argc; ++i)
		image_list.push_back(argv[i]);

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

	auto save_result = stitcher.savePanorama("output.jpg");
	if (save_result.isErr()) {
		std::cerr << save_result.error().toString() << '\n';
		return EXIT_FAILURE;
	}

	std::cout << "Panorama saved successfully.\n";
	return EXIT_SUCCESS;
}

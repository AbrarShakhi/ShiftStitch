#include <cstdlib>
#include <iostream>
#include <vector>

#include "shiftstitch/shiftstitch.hpp"
#include "shiftstitch/sift.hpp"

using namespace shiftstitch;

int main() {
	std::vector<std::string> front = {
	        "input/front/front_01.jpeg",
	        "input/front/front_02.jpeg",
	        "input/front/front_03.jpeg",
	};

	auto stitcherResult = ShiftStitcher::create(front);
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
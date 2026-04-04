#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <vector>

#include "shiftstitch/shiftstitch.hpp"
#include "shiftstitch/sift.hpp"

int main() {
	std::vector<std::string> images_path = {
	    "input/room/room01.jpeg",
	    "input/room/room02.jpeg",
	};

	shiftstitch::ShiftStitcher shiftstitch(images_path);

	shiftstitch::SIFT siftAlgorithm;
	shiftstitch.createPanorama(siftAlgorithm);

	shiftstitch.savePanorama("output/panorama.jpg");

	return EXIT_SUCCESS;
}
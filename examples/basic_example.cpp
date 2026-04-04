#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <vector>

#include "shiftstitch/shiftstitch.hpp"

int main() {
	std::vector<std::string> images_path = {
	    "input/room/room01.jpeg",
	    "input/room/room02.jpeg",
	};

	shiftstitch::ShiftStitcher shiftstitch(images_path);

	shiftstitch.createPanorama();
	shiftstitch.writePanorama("output/panorama.jpg");

	return EXIT_SUCCESS;
}
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <vector>

#include "shiftstitch/shiftstitch.hpp"
#include "shiftstitch/sift.hpp"

using namespace shiftstitch;

int main() {
	std::vector<std::string> room = {
	    "input/room/room01.jpeg",
	    "input/room/room02.jpeg",
	};

	std::vector<std::string> back = {
	    "input/back/back_01.jpeg",
	    "input/back/back_02.jpeg",
	    "input/back/back_03.jpeg",
	};

	std::vector<std::string> front = {
	    "input/front/front_01.jpeg",
	    "input/front/front_02.jpeg",
	    "input/front/front_03.jpeg",
	};

	ShiftStitcher shiftstitch(front);

	SIFT siftAlgorithm;
	shiftstitch.createPanorama(siftAlgorithm);

	shiftstitch.savePanorama("output.jpg");

	return EXIT_SUCCESS;
}
#include <cassert>

#include "shiftstitch/shiftstitch.hpp"

int main() {
	// Minimal test
	std::vector<std::string> images_path = {};

	shiftstitch::ShiftStitcher shiftstitch(images_path);
	return 0;
}
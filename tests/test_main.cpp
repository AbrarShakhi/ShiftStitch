#include <cassert>

#include "shiftstitch/shiftstitch.hpp"

int main() {
	// Minimal test
	std::vector<std::string> images_path = {};

	auto stitcherResult = shiftstitch::ShiftStitcher::create(images_path);
	return 0;
}
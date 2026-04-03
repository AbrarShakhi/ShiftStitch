#include "shiftstitch.hpp"
#include <cassert>

int main() {
    // Minimal test
    std::vector<cv::Mat> images;
    shiftstitch::ShiftStitcher stitcher;
    auto result = stitcher.createPanorama(images);

    assert(result.empty()); // placeholder assertion
    return 0;
}
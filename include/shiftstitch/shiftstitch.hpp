#pragma once

#include <opencv2/core.hpp>
#include <vector>

namespace shiftstitch {

class ShiftStitcher {
public:
    cv::Mat createPanorama(const std::vector<cv::Mat>& images);
};

}
#pragma once

#include <opencv2/core/mat.hpp>
#include <vector>

namespace shiftstitch {

class ISticher {
public:
	virtual cv::Mat stitch(std::vector<cv::Mat>& images) = 0;
};

}  // namespace shiftstitch

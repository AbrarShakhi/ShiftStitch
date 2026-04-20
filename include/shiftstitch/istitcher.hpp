#pragma once

#include <opencv2/core/mat.hpp>
#include <vector>

#include "result.hpp"

namespace shiftstitch {

class IStitcher {
public:
	virtual Result<cv::Mat> stitch(std::vector<cv::Mat>& images) = 0;
	virtual ~IStitcher() = default;
};

}  // namespace shiftstitch
#pragma once

#include "result.hpp"

#include <opencv2/core/mat.hpp>

namespace shiftstitch {

class IStitcher {
public:
	virtual Result<std::pair<cv::Mat, cv::Mat>> stitch(
	        const cv::Mat& query_photo, const cv::Mat& train_photo
	) = 0;
	virtual ~IStitcher() = default;
};

}  // namespace shiftstitch
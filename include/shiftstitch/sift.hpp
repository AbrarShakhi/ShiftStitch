#pragma once

#include "istitcher.hpp"
#include "result.hpp"

#include <opencv2/core/mat.hpp>
#include <opencv2/features2d.hpp>

namespace shiftstitch {

class SIFT : public IStitcher {
public:
	Result<std::pair<cv::Mat, cv::Mat>> stitch(
	        const cv::Mat& query_photo, const cv::Mat& train_photo
	) override;
};

}  // namespace shiftstitch

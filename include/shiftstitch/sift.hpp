#pragma once

#include <opencv2/core/mat.hpp>
#include <opencv2/features2d.hpp>
#include <vector>

#include "istitcher.hpp"
#include "result.hpp"

namespace shiftstitch {

class SIFT : public IStitcher {
public:
	Result<cv::Mat> stitch(std::vector<cv::Mat>& images) override;
};

}  // namespace shiftstitch

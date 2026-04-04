#pragma once

#include "shiftstitch/istitcher.hpp"

namespace shiftstitch {

class SIFT : public ISticher {
   public:
	cv::Mat stitch(std::vector<cv::Mat>&) override;
};

}  // namespace shiftstitch
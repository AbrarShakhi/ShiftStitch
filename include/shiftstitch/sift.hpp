#pragma once

#include "shiftstitch/istitcher.hpp"

namespace shiftstitch {

/**
 * Scale-Invariant Feature Transform (SIFT)
 */
class SIFT : public ISticher {
public:
	cv::Mat stitch(std::vector<cv::Mat>&) override;

private:
	void detectFeatures(
	        const cv::Mat& img, std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors
	);

	std::vector<cv::DMatch> matchFeatures(const cv::Mat& desc1, const cv::Mat& desc2);

	cv::Mat computeHomography(
	        const std::vector<cv::KeyPoint>& kp1,
	        const std::vector<cv::KeyPoint>& kp2,
	        const std::vector<cv::DMatch>& matches
	);

	cv::Mat warpAndBlend(const cv::Mat& base, const cv::Mat& newImg, const cv::Mat& H);
};

}  // namespace shiftstitch
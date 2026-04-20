#pragma once

#include <opencv2/core/mat.hpp>
#include <opencv2/features2d.hpp>
#include <vector>

#include "istitcher.hpp"
#include "result.hpp"

namespace shiftstitch {

class SIFT : public IStitcher {
public:
	Result<void> detectFeatures(
	        const cv::Mat& img, std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors
	) const;

	Result<std::vector<cv::DMatch>> matchFeatures(const cv::Mat& desc1, const cv::Mat& desc2) const;

	Result<cv::Mat> computeHomography(
	        const std::vector<cv::KeyPoint>& kp1,
	        const std::vector<cv::KeyPoint>& kp2,
	        const std::vector<cv::DMatch>& matches
	) const;

	Result<cv::Mat> warpAndBlend(const cv::Mat& base, const cv::Mat& newImg, const cv::Mat& H)
	        const;

	Result<cv::Mat> stitch(std::vector<cv::Mat>& images) override;
};

}  // namespace shiftstitch

#include "shiftstitch/sift.hpp"

#include <iostream>
#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

namespace shiftstitch {

void SIFT::detectFeatures(
        const cv::Mat& img, std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors
) {}

std::vector<cv::DMatch> SIFT::matchFeatures(const cv::Mat& desc1, const cv::Mat& desc2) {
	return std::vector<cv::DMatch>();
}

cv::Mat SIFT::computeHomography(
        const std::vector<cv::KeyPoint>& kp1,
        const std::vector<cv::KeyPoint>& kp2,
        const std::vector<cv::DMatch>& matches
) {
	return cv::Mat();
}

cv::Mat SIFT::warpAndBlend(const cv::Mat& base, const cv::Mat& newImg, const cv::Mat& H) {
	return cv::Mat();
}

cv::Mat SIFT::stitch(std::vector<cv::Mat>& images) {
	if (images.empty()) {
		return cv::Mat();
	}
	if (images.size() == 1) {
		return images[0];
	}

	cv::Mat result = images[0];

	for (size_t i = 1; i < images.size(); i++) {
		cv::Mat img = images[i];

		// 1. Detect features
		std::vector<cv::KeyPoint> kp1, kp2;
		cv::Mat desc1, desc2;

		detectFeatures(result, kp1, desc1);
		detectFeatures(img, kp2, desc2);

		// 2. Match
		std::vector<cv::DMatch> matches = matchFeatures(desc1, desc2);

		if (matches.size() < 10) {
			std::cerr << "Not enough matches!" << std::endl;
			continue;
		}

		// 3. Homography
		cv::Mat H = computeHomography(kp1, kp2, matches);

		if (H.empty()) {
			std::cerr << "Homography failed!" << std::endl;
			continue;
		}

		// 4. Warp + Blend
		result = warpAndBlend(result, img, H);
	}

	return result;
}

}  // namespace shiftstitch
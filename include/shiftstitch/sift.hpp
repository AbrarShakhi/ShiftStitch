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

private:
	void calculateWindowSize(const cv::Mat& query_photo, const cv::Mat& train_photo);

	std::pair<cv::Mat, cv::Mat> giveGray(const cv::Mat& image) const;

	static void siftDetector(
	        const cv::Mat& image, std::vector<cv::KeyPoint>& keypoints, cv::Mat& features
	);

	std::vector<cv::DMatch> createAndMatchKeypoints(
	        const cv::Mat& features_train, const cv::Mat& features_query
	) const;

	Result<void> computeHomography(
	        const std::vector<cv::KeyPoint>& kp_train,
	        const std::vector<cv::KeyPoint>& kp_query,
	        const std::vector<cv::DMatch>& matches,
	        double reprojThresh,
	        cv::Mat& H,
	        cv::Mat& status
	) const;

	cv::Mat blendingSmoothing(
	        const cv::Mat& query_image, const cv::Mat& train_image, const cv::Mat& H
	) const;

private:
	double smoothing_window_size_;

	cv::Mat createMask(
	        const cv::Mat& query_image, const cv::Mat& train_image, const std::string& version
	) const;
};

}  // namespace shiftstitch

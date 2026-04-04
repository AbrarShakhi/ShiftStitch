#include "shiftstitch/sift.hpp"

#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>

cv::Mat shiftstitch::SIFT::stitch(std::vector<cv::Mat>& images) {
	if (images.empty())
		return cv::Mat();

	cv::Mat result;
	cv::hconcat(images, result);

	return result;
}

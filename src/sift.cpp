#include "shiftstitch/sift.hpp"

#include "shiftstitch/result.hpp"

#include <iostream>
#include <opencv2/calib3d.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <utility>


namespace shiftstitch {

Result<std::pair<cv::Mat, cv::Mat>> SIFT::stitch(
        const cv::Mat& query_photo, const cv::Mat& train_photo
) {
	return Result<std::pair<cv::Mat, cv::Mat>>::Ok(std::make_pair(cv::Mat(), cv::Mat()));
}

}  // namespace shiftstitch
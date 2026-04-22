#include "shiftstitch/sift.hpp"

#include <iostream>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "shiftstitch/result.hpp"


namespace shiftstitch {

Result<cv::Mat> SIFT::stitch(std::vector<cv::Mat>& images) {
	return Result<cv::Mat>::Ok(result);
}

}  // namespace shiftstitch
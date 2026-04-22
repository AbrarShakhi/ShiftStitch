#pragma once

#include "shiftstitch/istitcher.hpp"
#include "shiftstitch/result.hpp"

#include <cstddef>
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <string>
#include <vector>

namespace shiftstitch {

class ShiftStitcher {
public:
	static Result<ShiftStitcher> create(const std::vector<std::string>& image_paths);
	static Result<ShiftStitcher> create(const std::string image_paths[], std::size_t size);

	Result<void> createPanorama(IStitcher& algorithm);
	Result<cv::Mat> toCvMat() const;
	Result<void> savePanorama(const std::string& output_path) const;

private:
	ShiftStitcher() = default;

	static Result<std::vector<cv::Mat>> loadImages(
	        const std::string* first, const std::string* last
	);

	std::vector<cv::Mat> images_mats_;
	cv::Mat panorama_;
	bool panorama_created_ = false;
};

}  // namespace shiftstitch

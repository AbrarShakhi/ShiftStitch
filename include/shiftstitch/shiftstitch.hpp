#pragma once

#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <vector>

namespace shiftstitch {

class ShiftStitcher {
   private:
	std::vector<std::string> &images_path;
	std::vector<cv::Mat> images_mat;
	bool isCreated;

   public:
	ShiftStitcher(std::vector<std::string> &images_path);
	~ShiftStitcher() = default;

	void createPanorama();
	cv::Mat toCvMat();
	void writePanorama(std::string output_path);

   private:
	std::vector<cv::Mat> loadImages();
};

}  // namespace shiftstitch
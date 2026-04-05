#pragma once

#include <cstddef>
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <vector>

#include "shiftstitch/istitcher.hpp"

namespace shiftstitch {

class ShiftStitcher {
   private:
	std::vector<cv::Mat> images_mats;
	cv::Mat panorama;
	bool panorama_created;

   public:
	ShiftStitcher(std::vector<std::string> &);
	ShiftStitcher(std::string[], std::size_t);
	~ShiftStitcher() = default;

	void createPanorama(ISticher &sticherAlgorithm);


	cv::Mat toCvMat();
	void savePanorama(std::string);

   private:
	template <typename It>
	std::vector<cv::Mat> loadImages(It, It);
};

}  // namespace shiftstitch

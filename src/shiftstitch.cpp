#include "shiftstitch/shiftstitch.hpp"

#include <iostream>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>


shiftstitch::ShiftStitcher::ShiftStitcher(
    std::vector<std::string>& images_paths) {
	panorama_created = false;
	images_mats = loadImages(images_paths.begin(), images_paths.end());
}

shiftstitch::ShiftStitcher::ShiftStitcher(std::string images_paths[],
                                          std::size_t size) {
	panorama_created = false;
	images_mats = loadImages(images_paths, images_paths + size);
}


template <typename It>
std::vector<cv::Mat> shiftstitch::ShiftStitcher::loadImages(It first, It end) {
	std::vector<cv::Mat> images_mats;

	for (auto it = first; it != end; ++it) {
		const std::string& impath = *it;

		cv::Mat img = cv::imread(impath);

		if (img.empty()) {
			throw std::runtime_error("Failed to load image: " + impath);
		}

		cv::cvtColor(img, img, cv::COLOR_BGR2RGB);

		images_mats.push_back(img);
	}

	return images_mats;
}


void shiftstitch::ShiftStitcher::createPanorama(ISticher& sticherAlgorithm) {
	std::cout << "Creating panorama..." << std::endl;

	if (images_mats.empty()) {
		throw std::runtime_error("No images loaded");
	}

	panorama = sticherAlgorithm.stitch(images_mats);
	panorama_created = true;
}


cv::Mat shiftstitch::ShiftStitcher::toCvMat() {
	if (!panorama_created) {
		throw std::runtime_error("Panorama has not been created yet");
	}
	return panorama;
}


void shiftstitch::ShiftStitcher::savePanorama(std::string output_path) {
	if (!panorama_created) {
		throw std::runtime_error("Panorama has not been created yet");
	}

	std::cout << "Writing to output: " << output_path << std::endl;
	cv::cvtColor(panorama, panorama, cv::COLOR_RGB2BGR);
	if (!cv::imwrite(output_path, panorama)) {
		throw std::runtime_error("Failed to write panorama to file");
	}
}

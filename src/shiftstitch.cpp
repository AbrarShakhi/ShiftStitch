#include "shiftstitch/shiftstitch.hpp"

#include <iostream>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>


shiftstitch::ShiftStitcher::ShiftStitcher(std::vector<std::string>& images_path)
    : images_paths(images_path) {
	isCreated = false;
	images_mats = loadImages();
}


std::vector<cv::Mat> shiftstitch::ShiftStitcher::loadImages() {
	std::vector<cv::Mat> images_mats;
	images_mats.reserve(images_paths.size());

	for (const auto& impath : images_paths) {
		cv::Mat img = cv::imread(impath);

		if (img.empty()) {
			throw std::runtime_error("Failed to load image: " + impath);
		}

		cv::cvtColor(img, img, cv::COLOR_BGR2RGB);

		images_mats.push_back(std::move(img));
	}

	return images_mats;
}


void shiftstitch::ShiftStitcher::createPanorama() {
	std::cout << "creating Panorama" << std::endl;
	isCreated = true;
}


cv::Mat shiftstitch::ShiftStitcher::toCvMat() {
	if (!isCreated) {
		throw std::runtime_error("Panorama has not created yet");
	}
	return cv::Mat();
}


void shiftstitch::ShiftStitcher::writePanorama(std::string output_path) {
	if (!isCreated) {
		throw std::runtime_error("Panorama has not created yet");
	}
	std::cout << "Writing to output" << std::endl;
}
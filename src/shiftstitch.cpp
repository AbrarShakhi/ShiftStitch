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
	panorama_created = false;
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

		images_mats.push_back(img);
	}

	return images_mats;
}


void shiftstitch::ShiftStitcher::createPanorama() {
	std::cout << "Creating panorama..." << std::endl;

	if (images_mats.empty()) {
		throw std::runtime_error("No images loaded");
	}

	// TODO: use SIFT algorith, to create panorama
	panorama = cv::Mat();
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
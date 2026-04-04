#include "shiftstitch/shiftstitch.hpp"

#include <iostream>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>


shiftstitch::ShiftStitcher::ShiftStitcher(std::vector<std::string> &images_path)
    : images_path(images_path) {
	isCreated = false;
	images_mat = loadImages();
}


std::vector<cv::Mat> shiftstitch::ShiftStitcher::loadImages() {
	return std::vector<cv::Mat>();
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
}
#include "shiftstitch/shiftstitch.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
  std::cout << "ShiftStitch example running!" << std::endl;

  // Minimal test of your library
  std::vector<cv::Mat> images;
  shiftstitch::ShiftStitcher stitcher;
  cv::Mat panorama = stitcher.createPanorama(images);

  std::cout << "Panorama size: " << panorama.size() << std::endl;
  return 0;
}
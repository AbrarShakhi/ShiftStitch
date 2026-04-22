#include "shiftstitch/shiftstitch.hpp"

#include <iostream>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <vector>

namespace shiftstitch {

Result<std::vector<cv::Mat>> ShiftStitcher::loadImages(
        const std::string* first, const std::string* last
) {
	std::vector<cv::Mat> mats;
	mats.reserve(std::distance(first, last));

	for (const auto* it = first; it != last; ++it) {
		cv::Mat img = cv::imread(*it);
		if (img.empty())
			return Result<std::vector<cv::Mat>>::Err(
			        {ErrorCode::ImageLoadFailed, "Failed to load image: " + *it}
			);
		mats.push_back(std::move(img));
	}

	return Result<std::vector<cv::Mat>>::Ok(std::move(mats));
}

Result<ShiftStitcher> ShiftStitcher::create(const std::vector<std::string>& image_paths) {
	return create(image_paths.data(), image_paths.size());
}

Result<ShiftStitcher> ShiftStitcher::create(const std::string image_paths[], std::size_t size) {
	if (size == 0)
		return Result<ShiftStitcher>::Err(
		        {ErrorCode::NoImagesProvided, "ShiftStitcher::create: image path list is empty"}
		);

	auto loadResult = loadImages(image_paths, image_paths + size);
	if (loadResult.isErr())
		return Result<ShiftStitcher>::Err(loadResult.error());

	ShiftStitcher s;
	s.images_mats_ = std::move(loadResult.value());
	return Result<ShiftStitcher>::Ok(std::move(s));
}

Result<void> ShiftStitcher::createPanorama(IStitcher& algorithm) {
	if (images_mats_.empty())
		return Result<void>::Err({ErrorCode::NoImagesProvided, "createPanorama: no images loaded"});

	if (images_mats_.size() < 2)
		return Result<void>::Err({ErrorCode::NoImagesProvided, "Need at least 2 images"});

	while (images_mats_.size() > 2) {
		auto stitch_result = algorithm
		                             .stitch(images_mats_[images_mats_.size() - 2],
		                                     images_mats_[images_mats_.size() - 1]);
		if (stitch_result.isErr())
			return Result<void>::Err(stitch_result.error());

		cv::Mat result_8u;
		stitch_result.value().first.convertTo(result_8u, CV_8UC3);

		images_mats_[images_mats_.size() - 2] = std::move(result_8u);
		images_mats_.pop_back();
	}

	auto stitch_result = algorithm.stitch(images_mats_[0], images_mats_[1]);
	if (stitch_result.isErr())
		return Result<void>::Err(stitch_result.error());

	auto [panorama, aux] = stitch_result.value();
	panorama_ = panorama;
	panorama_created_ = true;
	return Result<void>::Ok();
}

Result<cv::Mat> ShiftStitcher::toCvMat() const {
	if (!panorama_created_)
		return Result<cv::Mat>::Err(
		        {ErrorCode::PanoramaNotCreated, "toCvMat: panorama has not been created yet"}
		);
	return Result<cv::Mat>::Ok(panorama_);
}

Result<void> ShiftStitcher::savePanorama(const std::string& output_path) const {
	if (!panorama_created_)
		return Result<void>::Err(
		        {ErrorCode::PanoramaNotCreated, "savePanorama: panorama has not been created yet"}
		);

	std::cout << "Writing to: " << output_path << '\n';

	if (!cv::imwrite(output_path, panorama_))
		return Result<void>::Err(
		        {ErrorCode::SaveFailed, "savePanorama: cv::imwrite failed for path: " + output_path}
		);

	return Result<void>::Ok();
}

}  // namespace shiftstitch

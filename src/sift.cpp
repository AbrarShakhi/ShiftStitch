#include "shiftstitch/sift.hpp"

#include <iostream>
#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>
#include <vector>


namespace shiftstitch {

Result<void> SIFT::detectFeatures(
        const cv::Mat& img, std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors
) const {
	if (img.empty())
		return Result<void>::Err(
		        {ErrorCode::ImageLoadFailed, "detectFeatures: input image is empty"}
		);

	cv::Mat gray;
	if (img.channels() == 3)
		cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
	else if (img.channels() == 1)
		gray = img;
	else
		return Result<void>::Err(
		        {ErrorCode::ImageLoadFailed,
		         "detectFeatures: unexpected channel count " + std::to_string(img.channels())}
		);

	auto sift = cv::SIFT::create();
	sift->detectAndCompute(gray, cv::noArray(), keypoints, descriptors);

	return Result<void>::Ok();
}


Result<std::vector<cv::DMatch>> SIFT::matchFeatures(const cv::Mat& desc1, const cv::Mat& desc2)
        const {
	if (desc1.empty())
		return Result<std::vector<cv::DMatch>>::Err(
		        {ErrorCode::NotEnoughMatches, "matchFeatures: descriptor set 1 is empty"}
		);
	if (desc2.empty())
		return Result<std::vector<cv::DMatch>>::Err(
		        {ErrorCode::NotEnoughMatches, "matchFeatures: descriptor set 2 is empty"}
		);

	std::vector<std::vector<cv::DMatch>> knnMatches;
	cv::BFMatcher matcher(cv::NORM_L2);
	matcher.knnMatch(desc1, desc2, knnMatches, 2);

	const float ratioThresh = 0.75f;
	std::vector<cv::DMatch> goodMatches;

	for (const auto& m : knnMatches) {
		if (m.size() < 2)
			continue;
		if (m[0].distance < ratioThresh * m[1].distance)
			goodMatches.push_back(m[0]);
	}

	if (goodMatches.size() < 10)
		return Result<std::vector<cv::DMatch>>::Err(
		        {ErrorCode::NotEnoughMatches,
		         "matchFeatures: only " + std::to_string(goodMatches.size()) +
		                 " good matches found (need ≥10)"}
		);

	return Result<std::vector<cv::DMatch>>::Ok(std::move(goodMatches));
}


Result<cv::Mat> SIFT::computeHomography(
        const std::vector<cv::KeyPoint>& kp1,
        const std::vector<cv::KeyPoint>& kp2,
        const std::vector<cv::DMatch>& matches
) const {
	if (matches.size() < 4)
		return Result<cv::Mat>::Err(
		        {ErrorCode::HomographyFailed,
		         "computeHomography: need ≥4 matches, got " + std::to_string(matches.size())}
		);

	std::vector<cv::Point2f> pts1, pts2;
	pts1.reserve(matches.size());
	pts2.reserve(matches.size());

	for (const auto& m : matches) {
		pts1.push_back(kp1[m.queryIdx].pt);
		pts2.push_back(kp2[m.trainIdx].pt);
	}

	cv::Mat mask;
	cv::Mat H = cv::findHomography(pts2, pts1, cv::RANSAC, 4.0, mask);

	if (H.empty())
		return Result<cv::Mat>::Err(
		        {ErrorCode::HomographyFailed,
		         "computeHomography: findHomography returned empty matrix"}
		);


	int inliers = cv::countNonZero(mask);
	if (inliers < 4)
		return Result<cv::Mat>::Err(
		        {ErrorCode::HomographyFailed,
		         "computeHomography: only " + std::to_string(inliers) + " RANSAC inliers"}
		);

	return Result<cv::Mat>::Ok(H);
}


Result<cv::Mat> SIFT::warpAndBlend(const cv::Mat& base, const cv::Mat& newImg, const cv::Mat& H)
        const {
	if (base.empty())
		return Result<cv::Mat>::Err({ErrorCode::WarpFailed, "warpAndBlend: base image is empty"});
	if (newImg.empty())
		return Result<cv::Mat>::Err({ErrorCode::WarpFailed, "warpAndBlend: newImg is empty"});
	if (H.empty())
		return Result<cv::Mat>::Err({ErrorCode::WarpFailed, "warpAndBlend: homography is empty"});


	auto cornersOf = [](const cv::Mat& img) -> std::vector<cv::Point2f> {
		float c = (float)img.cols, r = (float)img.rows;
		return {{0, 0}, {c, 0}, {c, r}, {0, r}};
	};

	std::vector<cv::Point2f> baseCorners = cornersOf(base);
	std::vector<cv::Point2f> newImgCorners = cornersOf(newImg);
	std::vector<cv::Point2f> warpedCorners;
	cv::perspectiveTransform(newImgCorners, warpedCorners, H);

	float minX = std::numeric_limits<float>::max();
	float minY = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::lowest();
	float maxY = std::numeric_limits<float>::lowest();


	for (const auto& p : baseCorners) {
		minX = std::min(minX, p.x);
		minY = std::min(minY, p.y);
		maxX = std::max(maxX, p.x);
		maxY = std::max(maxY, p.y);
	}
	for (const auto& p : warpedCorners) {
		minX = std::min(minX, p.x);
		minY = std::min(minY, p.y);
		maxX = std::max(maxX, p.x);
		maxY = std::max(maxY, p.y);
	}

	int canvasW = (int)std::ceil(maxX - minX);
	int canvasH = (int)std::ceil(maxY - minY);

	if (canvasW <= 0 || canvasH <= 0)
		return Result<cv::Mat>::Err(
		        {ErrorCode::WarpFailed,
		         "warpAndBlend: degenerate canvas (" + std::to_string(canvasW) + "x" +
		                 std::to_string(canvasH) + ")"}
		);


	cv::Mat T = (cv::Mat_<double>(3, 3) << 1, 0, -minX, 0, 1, -minY, 0, 0, 1);


	cv::Mat warped;
	cv::warpPerspective(newImg, warped, T * H, cv::Size(canvasW, canvasH));


	cv::Mat newMask = cv::Mat::ones(newImg.size(), CV_8UC1) * 255;
	cv::Mat warpedMask;
	cv::warpPerspective(newMask, warpedMask, T * H, cv::Size(canvasW, canvasH));


	int roiX = (int)std::round(-minX);
	int roiY = (int)std::round(-minY);
	roiX = std::clamp(roiX, 0, canvasW - 1);
	roiY = std::clamp(roiY, 0, canvasH - 1);
	int roiW = std::min(base.cols, canvasW - roiX);
	int roiH = std::min(base.rows, canvasH - roiY);

	if (roiW <= 0 || roiH <= 0)
		return Result<cv::Mat>::Err(
		        {ErrorCode::WarpFailed, "warpAndBlend: base ROI is outside canvas bounds"}
		);

	cv::Mat canvas = warped.clone();
	cv::Mat canvasRoi(canvas, cv::Rect(roiX, roiY, roiW, roiH));


	cv::Mat baseCropped = base(cv::Rect(0, 0, roiW, roiH));


	cv::Mat overlapMask;
	cv::Mat warpedMaskRoi(warpedMask, cv::Rect(roiX, roiY, roiW, roiH));
	cv::threshold(warpedMaskRoi, overlapMask, 128, 255, cv::THRESH_BINARY);


	cv::Mat noOverlapMask;
	cv::bitwise_not(overlapMask, noOverlapMask);
	baseCropped.copyTo(canvasRoi, noOverlapMask);


	cv::Mat blended;
	cv::addWeighted(baseCropped, 0.5, canvasRoi, 0.5, 0, blended);
	blended.copyTo(canvasRoi, overlapMask);

	return Result<cv::Mat>::Ok(canvas);
}


Result<cv::Mat> SIFT::stitch(std::vector<cv::Mat>& images) {
	if (images.empty())
		return Result<cv::Mat>::Err({ErrorCode::NoImagesProvided, "stitch: no images provided"});

	if (images.size() == 1)
		return Result<cv::Mat>::Ok(images[0]);

	cv::Mat result = images[0];

	for (std::size_t i = 1; i < images.size(); ++i) {
		const cv::Mat& img = images[i];


		std::vector<cv::KeyPoint> kp1, kp2;
		cv::Mat desc1, desc2;

		auto r1 = detectFeatures(result, kp1, desc1);
		if (r1.isErr())
			return Result<cv::Mat>::Err(r1.error());

		auto r2 = detectFeatures(img, kp2, desc2);
		if (r2.isErr())
			return Result<cv::Mat>::Err(r2.error());


		auto matchResult = matchFeatures(desc1, desc2);
		if (matchResult.isErr()) {
			std::cerr << "[SIFT::stitch] image " << i << ": " << matchResult.error().message
			          << " — skipping\n";
			continue;
		}


		auto hResult = computeHomography(kp1, kp2, matchResult.value());
		if (hResult.isErr()) {
			std::cerr << "[SIFT::stitch] image " << i << ": " << hResult.error().message
			          << " — skipping\n";
			continue;
		}


		auto blendResult = warpAndBlend(result, img, hResult.value());
		if (blendResult.isErr())
			return Result<cv::Mat>::Err(blendResult.error());

		result = blendResult.value();
	}

	return Result<cv::Mat>::Ok(result);
}

}  // namespace shiftstitch

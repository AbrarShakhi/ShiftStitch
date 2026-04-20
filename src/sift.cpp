#include "shiftstitch/sift.hpp"

#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>


namespace shiftstitch {

Result<void> SIFT::detectFeatures(
        const cv::Mat& img, std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors
) const {
	if (img.empty())
		return Result<void>::Err({ErrorCode::ImageLoadFailed, "detectFeatures: empty image"});

	auto sift = cv::SIFT::create();
	sift->detectAndCompute(img, cv::noArray(), keypoints, descriptors);

	if (keypoints.empty() || descriptors.empty())
		return Result<void>::Err({ErrorCode::FeatureSelectionFailed, "No features detected"});

	return Result<void>::Ok();
}

Result<std::vector<cv::DMatch>> SIFT::matchFeatures(const cv::Mat& desc1, const cv::Mat& desc2)
        const {
	if (desc1.empty() || desc2.empty())
		return Result<std::vector<cv::DMatch>>::Err(
		        {ErrorCode::EmptyDescriptors, "Empty descriptors"}
		);

	cv::BFMatcher matcher(cv::NORM_L2);
	std::vector<std::vector<cv::DMatch>> knnMatches;
	matcher.knnMatch(desc1, desc2, knnMatches, 2);

	const float ratio_thresh = 0.75f;
	std::vector<cv::DMatch> goodMatches;

	for (const auto& m : knnMatches) {
		if (m.size() < 2)
			continue;
		if (m[0].distance < ratio_thresh * m[1].distance)
			goodMatches.push_back(m[0]);
	}

	if (goodMatches.size() < 4)
		return Result<std::vector<cv::DMatch>>::Err(
		        {ErrorCode::NotEnoughMatches, "Not enough good matches"}
		);

	return Result<std::vector<cv::DMatch>>::Ok(std::move(goodMatches));
}

Result<cv::Mat> SIFT::computeHomography(
        const std::vector<cv::KeyPoint>& kp1,
        const std::vector<cv::KeyPoint>& kp2,
        const std::vector<cv::DMatch>& matches
) const {
	if (matches.size() < 4)
		return Result<cv::Mat>::Err({ErrorCode::NotEnoughMatches, "Not enough matches"});

	std::vector<cv::Point2f> pts1, pts2;

	for (const auto& m : matches) {
		pts1.push_back(kp1[m.queryIdx].pt);
		pts2.push_back(kp2[m.trainIdx].pt);
	}

	cv::Mat H = cv::findHomography(pts2, pts1, cv::RANSAC);

	if (H.empty())
		return Result<cv::Mat>::Err({ErrorCode::HomographyFailed, "findHomography failed"});

	return Result<cv::Mat>::Ok(H);
}

Result<cv::Mat> SIFT::warpAndBlend(const cv::Mat& base, const cv::Mat& newImg, const cv::Mat& H)
        const {
	if (base.empty() || newImg.empty() || H.empty())
		return Result<cv::Mat>::Err({ErrorCode::WarpFailed, "Invalid input to warpAndBlend"});

	cv::Mat warped;
	cv::warpPerspective(newImg, warped, H, cv::Size(base.cols + newImg.cols, base.rows));

	cv::Mat result = warped.clone();
	base.copyTo(result(cv::Rect(0, 0, base.cols, base.rows)));

	return Result<cv::Mat>::Ok(result);
}

Result<cv::Mat> SIFT::stitch(std::vector<cv::Mat>& images) {
	if (images.size() < 2)
		return Result<cv::Mat>::Err({ErrorCode::NoImagesProvided, "Need at least 2 images"});

	cv::Mat panorama = images[0];

	for (size_t i = 1; i < images.size(); ++i) {
		std::vector<cv::KeyPoint> kp1, kp2;
		cv::Mat desc1, desc2;

		auto res1 = detectFeatures(panorama, kp1, desc1);
		if (res1.isErr())
			return Result<cv::Mat>::Err(res1.error());

		auto res2 = detectFeatures(images[i], kp2, desc2);
		if (res2.isErr())
			return Result<cv::Mat>::Err(res2.error());

		auto matchRes = matchFeatures(desc1, desc2);
		if (matchRes.isErr())
			return Result<cv::Mat>::Err(matchRes.error());

		auto Hres = computeHomography(kp1, kp2, matchRes.value());
		if (Hres.isErr())
			return Result<cv::Mat>::Err(Hres.error());

		auto warpRes = warpAndBlend(panorama, images[i], Hres.value());
		if (warpRes.isErr())
			return Result<cv::Mat>::Err(warpRes.error());

		panorama = warpRes.value();
	}

	return Result<cv::Mat>::Ok(panorama);
}

}  // namespace shiftstitch
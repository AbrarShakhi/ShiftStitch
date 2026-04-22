#include "shiftstitch/sift.hpp"

#include "shiftstitch/result.hpp"

#include <opencv2/calib3d.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <utility>


namespace shiftstitch {

Result<std::pair<cv::Mat, cv::Mat>> SIFT::stitch(
        const cv::Mat& query_photo, const cv::Mat& train_photo
) {
	calculateWindowSize(query_photo, train_photo);

	// Grayscale
	auto [query_color, query_gray] = giveGray(query_photo);
	auto [train_color, train_gray] = giveGray(train_photo);

	// SIFT
	std::vector<cv::KeyPoint> kp_train, kp_query;
	cv::Mat feat_train, feat_query;
	siftDetector(train_gray, kp_train, feat_train);
	siftDetector(query_gray, kp_query, feat_query);

	// Match
	auto matches = createAndMatchKeypoints(feat_train, feat_query);

	// Draw top-100 matches for the debug image
	int draw_count = std::min((int)matches.size(), 100);
	std::vector<cv::DMatch> top_matches(matches.begin(), matches.begin() + draw_count);

	cv::Mat mapped_feature_image;
	cv::drawMatches(
	        train_photo,
	        kp_train,
	        query_photo,
	        kp_query,
	        top_matches,
	        mapped_feature_image,
	        cv::Scalar::all(-1),
	        cv::Scalar::all(-1),
	        std::vector<char>(),
	        cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS
	);

	// Homography
	cv::Mat H, status;
	auto homography_result = computeHomography(kp_train, kp_query, matches, 4.0, H, status);
	if (homography_result.isErr()) {
		return Result<std::pair<cv::Mat, cv::Mat>>::Err(homography_result.error());
	}

	// Blend
	cv::Mat result = blendingSmoothing(query_photo, train_photo, H);

	cv::Mat result_f32, result_rgb;
	result.convertTo(result_f32, CV_32FC3);
	// cv::cvtColor(result_f32, result_rgb, cv::COLOR_BGR2RGB);

	// // Convert mapped image to float32 RGB
	cv::Mat mapped_f32, mapped_rgb;
	mapped_feature_image.convertTo(mapped_f32, CV_32FC3);
	// cv::cvtColor(mapped_f32, mapped_rgb, cv::COLOR_BGR2RGB);

	// return Result<std::pair<cv::Mat, cv::Mat>>::Ok({result_f32, mapped_rgb});
	return Result<std::pair<cv::Mat, cv::Mat>>::Ok({result_f32, mapped_f32});
	// return Result<std::pair<cv::Mat, cv::Mat>>::Ok({result, mapped_feature_image});
}



void SIFT::calculateWindowSize(const cv::Mat& query_photo, const cv::Mat& train_photo) {
	int width_query = query_photo.cols;
	int width_train = train_photo.cols;
	int lowest_width = std::min(width_query, width_train);

	const double smoothing_window_percent = 0.10;
	double raw = smoothing_window_percent * lowest_width;
	smoothing_window_size_ = std::max(100.0, std::min(raw, 1000.0));
}



std::pair<cv::Mat, cv::Mat> SIFT::giveGray(const cv::Mat& image) const {
	cv::Mat gray;
	cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
	return {image, gray};
}



void SIFT::siftDetector(
        const cv::Mat& image, std::vector<cv::KeyPoint>& keypoints, cv::Mat& features
) {
	auto descriptor = cv::SIFT::create();
	descriptor->detectAndCompute(image, cv::noArray(), keypoints, features);
}



std::vector<cv::DMatch> SIFT::createAndMatchKeypoints(
        const cv::Mat& features_train, const cv::Mat& features_query
) const {
	cv::BFMatcher bf(cv::NORM_L2, /*crossCheck=*/true);
	std::vector<cv::DMatch> matches;
	bf.match(features_train, features_query, matches);
	std::sort(matches.begin(), matches.end(), [](const cv::DMatch& a, const cv::DMatch& b) {
		return a.distance < b.distance;
	});
	return matches;
}



Result<void> SIFT::computeHomography(
        const std::vector<cv::KeyPoint>& kp_train,
        const std::vector<cv::KeyPoint>& kp_query,
        const std::vector<cv::DMatch>& matches,
        double reprojThresh,
        cv::Mat& H,
        cv::Mat& status
) const {
	if (matches.size() < 4) {
		return Result<void>::Err(
		        {ErrorCode::HomographyFailed,
		         "Minimum match count not satisfied, cannot get homography"}
		);
	}

	std::vector<cv::Point2f> pts_train, pts_query;
	pts_train.reserve(matches.size());
	pts_query.reserve(matches.size());

	for (const auto& m : matches) {
		pts_train.push_back(kp_train[m.queryIdx].pt);
		pts_query.push_back(kp_query[m.trainIdx].pt);
	}

	H = cv::findHomography(pts_train, pts_query, cv::RANSAC, reprojThresh, status);
	return (!H.empty()) ? Result<void>::Ok()
	                    : Result<void>::Err(
	                              {ErrorCode::HomographyFailed,
	                               "cannot stitch images (homography failed)"}
	                      );
}



cv::Mat SIFT::blendingSmoothing(
        const cv::Mat& query_image, const cv::Mat& train_image, const cv::Mat& H
) const {
	int h_pano = query_image.rows;
	int w_pano = query_image.cols + train_image.cols;


	cv::Mat panorama1 = cv::Mat::zeros(h_pano, w_pano, CV_64FC3);
	cv::Mat query64;
	query_image.convertTo(query64, CV_64FC3);
	query64.copyTo(panorama1(cv::Rect(0, 0, query_image.cols, query_image.rows)));

	cv::Mat mask1 = createMask(query_image, train_image, "left_image");
	cv::multiply(panorama1, mask1, panorama1);


	cv::Mat warped;
	cv::Mat train64;
	train_image.convertTo(train64, CV_64FC3);
	cv::warpPerspective(train64, warped, H, cv::Size(w_pano, h_pano));

	cv::Mat mask2 = createMask(query_image, train_image, "right_image");
	cv::Mat panorama2;
	cv::multiply(warped, mask2, panorama2);


	cv::Mat result = panorama1 + panorama2;


	cv::Mat gray_result;
	{
		cv::Mat result8;
		result.convertTo(result8, CV_8UC3);
		cv::cvtColor(result8, gray_result, cv::COLOR_RGB2GRAY);
	}

	int min_row = h_pano, max_row = 0;
	int min_col = w_pano, max_col = 0;

	for (int r = 0; r < gray_result.rows; ++r) {
		for (int c = 0; c < gray_result.cols; ++c) {
			if (gray_result.at<uchar>(r, c) != 0) {
				min_row = std::min(min_row, r);
				max_row = std::max(max_row, r);
				min_col = std::min(min_col, c);
				max_col = std::max(max_col, c);
			}
		}
	}

	cv::Mat final_result =
	        result(cv::Range(min_row, max_row + 1), cv::Range(min_col, max_col + 1)).clone();

	return final_result;
}



cv::Mat SIFT::createMask(
        const cv::Mat& query_image, const cv::Mat& train_image, const std::string& version
) const {
	int h_pano = query_image.rows;
	int w_query = query_image.cols;
	int w_train = train_image.cols;
	int w_pano = w_query + w_train;

	int offset = static_cast<int>(smoothing_window_size_ / 2);
	int barrier = w_query - offset;


	cv::Mat mask = cv::Mat::zeros(h_pano, w_pano, CV_64F);


	int ramp_len = 2 * offset;
	cv::Mat ramp(1, ramp_len, CV_64F);
	for (int i = 0; i < ramp_len; ++i) {
		if (version == "left_image")
			ramp.at<double>(0, i) = 1.0 - static_cast<double>(i) / (ramp_len - 1);
		else
			ramp.at<double>(0, i) = static_cast<double>(i) / (ramp_len - 1);
	}


	int col_start = barrier - offset;
	int col_end = barrier + offset;
	col_start = std::max(col_start, 0);
	col_end = std::min(col_end, w_pano);

	for (int r = 0; r < h_pano; ++r) {
		for (int c = col_start; c < col_end; ++c) {
			int idx = c - (barrier - offset);
			if (idx >= 0 && idx < ramp_len)
				mask.at<double>(r, c) = ramp.at<double>(0, idx);
		}
	}


	if (version == "left_image") {
		for (int r = 0; r < h_pano; ++r)
			for (int c = 0; c < col_start; ++c)
				mask.at<double>(r, c) = 1.0;
	} else {
		for (int r = 0; r < h_pano; ++r)
			for (int c = col_end; c < w_pano; ++c)
				mask.at<double>(r, c) = 1.0;
	}


	std::vector<cv::Mat> channels = {mask, mask, mask};
	cv::Mat mask3;
	cv::merge(channels, mask3);
	return mask3;
}

}  // namespace shiftstitch
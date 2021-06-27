#pragma once

#include <z/core/array.hpp>
#include <opencv2/core/core.hpp>

z::core::array<cv::Rect> pointsOfInterest(const cv::Mat& image);

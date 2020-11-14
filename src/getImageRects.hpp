#pragma once

#include <opencv2/core/core.hpp>

void getImageRects(cv::Mat& image, std::vector<cv::Rect>& wordBank, std::vector<cv::Rect>& letters) noexcept;

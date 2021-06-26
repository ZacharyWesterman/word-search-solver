#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

void encircle(cv::Mat& image, const cv::Point& p1, const cv::Point& p2, const cv::Scalar& color, int thickness, int radius) noexcept;

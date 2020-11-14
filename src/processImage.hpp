#pragma once

#include <opencv2/core/core.hpp>
#include "wordSearch.hpp"

cv::Mat processImage(const cv::Mat& image, wordSearch& grid, bool resize = true, bool log = true);

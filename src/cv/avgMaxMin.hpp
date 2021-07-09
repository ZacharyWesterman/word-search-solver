#pragma once

#include <z/core/array.hpp>
#include <opencv2/core/core.hpp>

//Get average, min, and max distance between ordered points of interest.
//returns false if the difference between max and min is negligible, true if it's significant
bool avgMaxMin(const z::core::array<cv::Rect>& rects, const z::core::array<int>& connections, int& average, int& min, int& max);

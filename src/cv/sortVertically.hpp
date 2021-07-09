#pragma once

#include <z/core/array.hpp>
#include <opencv2/core/core.hpp>

z::core::array<z::core::array<int>> sortVertically(const z::core::array<cv::Rect>& rects, const z::core::array<z::core::array<int>>& connections);

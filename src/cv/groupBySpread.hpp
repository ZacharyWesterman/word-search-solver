#pragma once

#include <z/core/array.hpp>
#include <opencv2/core/core.hpp>

//Get a group of organized rows of PoI based on whether they are consistently spaced or not.
z::core::array<z::core::array<int>> groupBySpread(const z::core::array<cv::Rect>& rects, const z::core::array<z::core::array<int>>& connections, bool significant);

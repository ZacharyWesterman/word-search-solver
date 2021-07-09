#pragma once

#include <z/core/array.hpp>
#include <opencv2/core/core.hpp>

/**
 * \brief Connect rectangles that are fully separate and roughly horizontal to each other.
 *
 * Gives an array (X) of arrays (Y) of rect IDs, ordered such that each Y has a single row
 * of IDs, ordered by the rects' horizontal position. X is ordered vertically.
 */
z::core::array<z::core::array<int>> connectNearest(const z::core::array<cv::Rect>& rects);

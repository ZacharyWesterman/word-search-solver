#pragma once

#include <opencv2/core/core.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include <z/core.hpp>

z::core::array< z::core::string<z::utf8> > ocrWords(const cv::Mat& image, const std::vector<cv::Rect>& wordBank_rects) noexcept;
z::core::array<z::core::string<z::utf8> > ocrLetters(const cv::Mat& image, const std::vector<cv::Rect>& rects) noexcept;

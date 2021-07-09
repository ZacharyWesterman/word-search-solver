#pragma once

#include <opencv2/core/core.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include <z/core.hpp>

z::core::array<zstring> ocrWords(tesseract::TessBaseAPI* ocr, const cv::Mat& image, const z::core::array<cv::Rect>& rects) noexcept;
z::core::array<zstring> ocrLetters(tesseract::TessBaseAPI* ocr, const cv::Mat& image, const z::core::array<cv::Rect>& rects) noexcept;

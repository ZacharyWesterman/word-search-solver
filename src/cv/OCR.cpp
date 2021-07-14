#include "OCR.hpp"
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include <opencv2/imgproc/imgproc.hpp>

static zstring recognize(const cv::Mat& image, cv::Rect rect, tesseract::TessBaseAPI* ocr, bool oneLetter = false) noexcept
{
	zstring word;

	cv::Mat im (image, rect);
	//Convert to grayscale
	// cv::Mat im;
	// cv::cvtColor(im0, im, CV_BGR2GRAY);

	//scale to something manageable
	// if (im.rows > 20)
	// {
	// 	double scale = 20.0 / im.rows;
	// 	cv::Mat im2 = im;
	// 	cv::resize(im2, im, cv::Size(), scale, scale, cv::INTER_AREA);
	// }
	// if (im.rows < 20)
	// {
	// 	double scale = 20.0 / im.rows;
	// 	cv::Mat im2 = im;
	// 	cv::resize(im2, im, cv::Size(), scale, scale, cv::INTER_NEAREST);
	// }

	ocr->SetImage(im.data, im.cols, im.rows, 3, im.step);
	ocr->SetSourceResolution(300);

	word = ocr->GetUTF8Text();
	word.toUpper();

	word = word.trim().filter('A','Z');

	if (oneLetter)
	{
		if (word[0] == word[1]) word = word[0];
		if (word[0] == 'I' && word[1]) word = word[1];
	}

	return word;
}

z::core::array<zstring> ocrWords(tesseract::TessBaseAPI* ocr, const cv::Mat& image, const z::core::array<cv::Rect>& rects) noexcept
{
	z::core::array<zstring> result;

	//Recognize word bank
	ocr->SetPageSegMode(tesseract::PSM_SINGLE_WORD);
	for (auto rect : rects)
	{
		auto str = recognize(image, rect, ocr);
		if (str.length() > 2) result.add(str);
	}

	return result;
}

z::core::array<zstring> ocrLetters(tesseract::TessBaseAPI* ocr, const cv::Mat& image, const z::core::array<cv::Rect>& rects) noexcept
{
	z::core::array<zstring> result;

	//Recognize letters
	ocr->SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
	for (auto rect : rects)
	{
		result.add(recognize(image, rect, ocr, true)[0]);
	}

	return result;
}

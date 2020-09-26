#include "OCR.hpp"
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include <opencv2/imgproc/imgproc.hpp>

static z::core::string<z::utf8> recognize(const cv::Mat& image, cv::Rect rect, tesseract::TessBaseAPI* ocr) noexcept
{
	z::core::string<z::utf8> word;

	cv::Mat im (image, rect);

	//scale to something manageable
	if (im.rows > 20)
	{
		double scale = 20.0 / im.rows;
		cv::Mat im2 = im;
		cv::resize(im2, im, cv::Size(), scale, scale, cv::INTER_AREA);
	}

	ocr->SetImage(im.data, im.cols, im.rows, 3, im.step);
	ocr->SetSourceResolution(70);

	word = ocr->GetUTF8Text();
	// return word.toUpper();
	return word.toUpper().filter('A','Z');
}

z::core::array<z::core::string<z::utf8> > ocrWords(const cv::Mat& image, const std::vector<cv::Rect>& rects) noexcept
{
	z::core::array<z::core::string<z::utf8> > result;

	//Start up OCR
	tesseract::TessBaseAPI* ocr = new tesseract::TessBaseAPI();
	ocr->Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);

	//Recognize word bank
	ocr->SetPageSegMode(tesseract::PSM_SINGLE_WORD);
	for (auto rect : rects)
	{
		result.add(recognize(image, rect, ocr));
	}

	ocr->End();

	return result;
}

z::core::array<z::core::string<z::utf8> > ocrLetters(const cv::Mat& image, const std::vector<cv::Rect>& rects) noexcept
{
	z::core::array<z::core::string<z::utf8> > result;

	//Start up OCR
	tesseract::TessBaseAPI* ocr = new tesseract::TessBaseAPI();
	ocr->Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);

	//Recognize letters
	int lastY = image.cols;
	ocr->SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
	for (auto rect : rects)
	{
		result.add(recognize(image, rect, ocr)[0]);
	}

	ocr->End();

	return result;
}

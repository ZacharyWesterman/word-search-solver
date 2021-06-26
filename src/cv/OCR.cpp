#include "OCR.hpp"
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include <opencv2/imgproc/imgproc.hpp>

//TEMP headers
// #include <z/system/stdout.hpp>
// #include <opencv2/core/core.hpp>
// #include <opencv2/highgui/highgui.hpp>
// #include <opencv2/imgproc/imgproc.hpp>
// #include <opencv2/objdetect.hpp>

static z::core::string<z::utf8> recognize(const cv::Mat& image, cv::Rect rect, tesseract::TessBaseAPI* ocr, bool oneLetter = false) noexcept
{
	z::core::string<z::utf8> word;

	//Convert to grayscale

	cv::Mat im (image, rect);
	// cv::Mat im;
	// cv::cvtColor(im0, im, CV_BGR2GRAY);

	//scale to something manageable
	if (im.rows > 20)
	{
		double scale = 20.0 / im.rows;
		cv::Mat im2 = im;
		cv::resize(im2, im, cv::Size(), scale, scale, cv::INTER_AREA);
	}
	else if (im.rows < 20)
	{
		double scale = 20.0 / im.rows;
		cv::Mat im2 = im;
		cv::resize(im2, im, cv::Size(), scale, scale, cv::INTER_NEAREST);
	}

	ocr->SetImage(im.data, im.cols, im.rows, 3, im.step);
	ocr->SetSourceResolution(300);

	word = ocr->GetUTF8Text();
	word.toUpper();

	//replace similar-looking characters.
	//kinda a hack but tweaking recognition params is a pita.
	word.replace('|','I');
	word.replace('1','I');
	word.replace('0','O');
	word.replace(L"°","O");
	word.replace(L"¢","C");
	word.replace('=','E');

	// z::system::stdout stdout;
	// word.trim().writeln(stdout);
	word = word.trim().filter('A','Z');

	if (oneLetter)
	{
		if (word[0] == word[1]) word = word[0];
		if (word[0] == 'I' && word[1]) word = word[1];
	}

	// if (forceMatch && !word[0]) word = 'I'; //sometimes singular "I" chars are not detected as anything.


	// cv::namedWindow("Puzzle", cv::WINDOW_AUTOSIZE);
	// cv::imshow("Puzzle", im);
	// cv::waitKey(0);
	// return word.toUpper();
	return word;
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
	// ocr->SetVariable("tessedit_char_whitelist", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ|10");
	for (auto rect : rects)
	{
		result.add(recognize(image, rect, ocr, true));
	}

	ocr->End();

	return result;
}

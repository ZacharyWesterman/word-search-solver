#include <z/file.hpp>
#include <z/system.hpp>
#include <z/core.hpp>
#include <z/util.hpp>

//Image processing
// #include "cv/processImage.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

//for letting this thread sleep
#include <chrono>
#include <thread>

//Image processing
#include "cv/colors.hpp"
#include "cv/pointsOfInterest.hpp"
#include "cv/connectNearest.hpp"
#include "cv/groupBySpread.hpp"
#include "cv/buildWordRects.hpp"
#include "cv/buildLetterRects.hpp"
#include "cv/sortVertically.hpp"
#include "cv/encircle.hpp"

//Text recognition
#include "cv/OCR.hpp"

//Word search
#include "shared/wordSearch.hpp"

int main(int argc, char** argv)
{
	z::system::stderr stderr;
	z::system::stdout stdout;
	(void)argc;
	(void)argv;

	if (argc < 2)
	{
		zstring("Error: Input image required.").writeln(stderr);
		return 1;
	}

	std::string imagePath = argv[1];
	cv::Mat image = cv::imread(imagePath, CV_LOAD_IMAGE_COLOR);
	if (!image.data)
	{
		(zstring("Error: Unable to load ")+imagePath.c_str()).writeln(stderr);
		return 2;
	}

	//get points of interest
	auto rects = pointsOfInterest(image);

	//Connect points of interest in order horizontally.
	auto connections = connectNearest(rects);

	//get groups that are most likely in the word bank.
	auto wordRects = buildWordRects(rects, groupBySpread(rects, connections, false));

	//get groups that are most likely inside the puzzle
	auto letterRects = buildLetterRects(rects, sortVertically(rects, groupBySpread(rects, connections, true)));

	//Start up text recognition
	tesseract::TessBaseAPI* ocr = new tesseract::TessBaseAPI();
	ocr->Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);

	//Build puzzle text
	zstring letterText;
	for (const auto& row : letterRects)
	{
		auto letters = ocrLetters(ocr, image, row);
		if (letterText.length()) letterText += "\n";
		letterText += z::core::join(letters, "");
	}
	std::cout << letterText << std::endl << std::endl;

	//Build word bank
	auto words = ocrWords(ocr, image, wordRects);
	// std::cout << z::core::join(words, "\n") << std::endl;

	//Done with text recognition
	ocr->End();

	//init word search
	wordSearch wordsearch;
	wordsearch.load(letterText);

	//highlight word bank and successful matches
	for (int i=0; i<words.length(); ++i)
	{
		if (!wordsearch.find(words[i])) continue; //do nothing if unable to find the word

		auto match = wordsearch.getMatchData(0);

		auto begRect = letterRects[match.y][match.x];
		auto endRect = letterRects[match.height][match.width];

		cv::Point p1 (begRect.x + (begRect.width / 2), begRect.y + (begRect.height / 2));
		cv::Point p2 (endRect.x + (endRect.width / 2), endRect.y + (endRect.height / 2));

		encircle(image, p1, p2, color(i), 2, (begRect.width + begRect.height + endRect.width + endRect.height)/8);
		cv::rectangle(image, wordRects[i], color(i), 2);
	}

	//Resize image to something manageable
	if (image.rows > 1024)
	{
		double scale = 1024.0 / (double)image.rows; //scale height down to 1024;
		cv::Mat img2 = image;
		cv::resize(img2, image, cv::Size(), scale, scale, cv::INTER_AREA);
	}
	if (image.cols > 1024)
	{
		double scale = 1024.0 / (double)image.cols; //scale height down to 1024;
		cv::Mat img2 = image;
		cv::resize(img2, image, cv::Size(), scale, scale, cv::INTER_AREA);
	}

	//Open window and display
	cv::namedWindow("Puzzle", cv::WINDOW_AUTOSIZE);
	cv::imshow("Puzzle", image);
	cv::waitKey(0);
}

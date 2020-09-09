#include <z/file.hpp>
#include <z/system.hpp>
#include <z/core.hpp>
#include <z/util.hpp>

//Image processing
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect.hpp>

//OCR
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

//for letting this thread sleep
#include <chrono>
#include <thread>

#include <algorithm> //std::sort

//Word search
#include "wordSearch.hpp"

bool rectContains(const cv::Rect& rect1, const cv::Rect& rect2)
{
	bool xmin = (rect1.x <= rect2.x);
	bool ymin = (rect1.y <= rect2.y);
	bool xmax = (rect1.x + rect1.width) >= (rect2.x + rect2.width);
	bool ymax = (rect1.y + rect1.height) >= (rect2.y + rect2.height);

	return xmin && ymin && xmax && ymax;
}

bool rectCompare(const cv::Rect& l, const cv::Rect& r)
{
	float cellSize = 10.0;
	float lCell = floorf(l.y / cellSize);
	float rCell = floorf(r.y / cellSize);
	if(lCell == rCell) return l.x > r.x;
	return (lCell > rCell);
}

void getImageRects(cv::Mat& image, std::vector<cv::Rect>& wordBank, std::vector<cv::Rect>& letters)
{
	//Convert to grayscale
	cv::Mat grayImage;
	cv::cvtColor(image, grayImage, CV_BGR2GRAY);

	//Morphological gradient
	cv::Mat grad;
	cv::Mat morphKernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
	cv::morphologyEx(grayImage, grad, cv::MORPH_GRADIENT, morphKernel);

	//Binarize
	cv::Mat bw;
	cv::threshold(grad, bw, 0.0, 255.0, cv::THRESH_BINARY | cv::THRESH_OTSU);


	// connect horizontally oriented regions
	cv::Mat connected;
	morphKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9, 1));
	cv::morphologyEx(bw, connected, cv::MORPH_CLOSE, morphKernel);


	// find disconnected contours
	cv::Mat mask = cv::Mat::zeros(bw.size(), CV_8UC1);
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(bw, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	// //find connected contours
	std::vector<std::vector<cv::Point>> contours_connected;
	std::vector<cv::Vec4i> hierarchy2;
	cv::findContours(connected, contours_connected, hierarchy2, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));


	// filter disconnected contours
	std::vector<cv::Rect> interestPoints;
	cv::Rect bounds;
	for(int idx = 0; idx >= 0; idx = hierarchy[idx][0])
	{
		cv::Rect rect = cv::boundingRect(contours[idx]);
		cv::Mat maskROI(mask, rect);
		maskROI = cv::Scalar(0, 0, 0);
		// fill the contour
		cv::drawContours(mask, contours, idx, cv::Scalar(255, 255, 255), CV_FILLED);

		auto volume = rect.height * rect.width;
		if ((volume > 10) && (volume < 10000))
		{
			//filter out rects that are contained by other rects
			bool filter = false;
			int i = 0;
			for (int i=0; i<interestPoints.size(); ++i)
			{
				if (rectContains(interestPoints[i], rect))
				{
					filter = true;
				}
				else if (rectContains(rect,interestPoints[i]))
				{
					interestPoints.erase(interestPoints.begin()+i);
					--i;
				}
			}

			if (!filter) interestPoints.push_back(rect);
		}
		else if (volume > 10) bounds = rect;
	}

	//filter connected contours
	std::vector<cv::Rect> interestPoints2;
	for(int idx = 0; idx >= 0; idx = hierarchy2[idx][0])
	{
		cv::Rect rect = cv::boundingRect(contours_connected[idx]);
		cv::Mat maskROI(mask, rect);
		maskROI = cv::Scalar(0, 0, 0);
		// fill the contour
		cv::drawContours(mask, contours_connected, idx, cv::Scalar(255, 255, 255), CV_FILLED);

		auto volume = rect.height * rect.width;
		if ((volume > 10) && (volume < 10000))
		{
			interestPoints2.push_back(rect);
		}
	}

	//letters that are outside the determined bounds are part of the word bank

	for (auto& rect : interestPoints)
	{
		bool xmin = (rect.x > bounds.x);
		bool ymin = (rect.y > bounds.y);
		bool xmax = (rect.x + rect.width) < (bounds.x + bounds.width);
		bool ymax = (rect.y + rect.height) < (bounds.y + bounds.height);
		if (xmin && ymin && xmax && ymax)
		{
			letters.push_back(rect);
			// cv::rectangle(image, rect, cv::Scalar(0, 255, 0), 2);
		}
	}

	for (auto& rect : interestPoints2)
	{
		bool xmin = (rect.x > bounds.x);
		bool ymin = (rect.y > bounds.y);
		bool xmax = (rect.x + rect.width) < (bounds.x + bounds.width);
		bool ymax = (rect.y + rect.height) < (bounds.y + bounds.height);
		if (!(xmin && ymin && xmax && ymax))
		{
			wordBank.push_back(rect);
			// cv::rectangle(image, rect, cv::Scalar(0, 155, 100), 2);
		}
	}

	// cv::rectangle(image, bounds, cv::Scalar(255, 0, 0), 2);

	//sort letters by position
	std::sort(letters.begin(), letters.end(), rectCompare);
}

int main(int argc, char** argv)
{
	z::system::stderr stderr;
	z::system::stdout stdout;
	(void)argc;
	(void)argv;

	if (argc < 2)
	{
		zstring("Input image required.").writeln(stderr);
		return 1;
	}

	std::string outText;
	std::string imagePath = argv[1];
	cv::Mat image = cv::imread(imagePath, CV_LOAD_IMAGE_COLOR);
	if (!image.data)
	{
		(zstring("Unable to load ")+imagePath.c_str()).writeln(stderr);
		return 2;
	}

	//Resize image to something manageable

	// if (image.rows > 1024)
	// {
	// 	double scale = 1024.0 / (double)image.rows; //scale height down to 1024;
	// 	cv::Mat img2 = image;
	// 	cv::resize(img2, image, cv::Size(), scale, scale, cv::INTER_AREA);
	// }


	std::vector<cv::Rect> wordBank_rects;
	std::vector<cv::Rect> letter_rects;
	getImageRects(image, wordBank_rects, letter_rects);

	/* create smaller image */
	// cv::Mat boundMat (image, bounds);

	//Open windows
	// cv::namedWindow("Puzzle", cv::WINDOW_AUTOSIZE);
	// cv::imshow("Puzzle", image);
	// cv::waitKey(0);
	// return 0;
	/* and terminate */


	z::core::array<z::core::string<z::utf8> > wordBank;
	z::core::array<z::core::string<z::utf8> > letters;
	int letterCount = 0;
	int letterRows = 1;

	//Start up OCR
	tesseract::TessBaseAPI* ocr = new tesseract::TessBaseAPI();
	ocr->Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);

	//Recognize word bank
	ocr->SetPageSegMode(tesseract::PSM_SINGLE_WORD);
	for (auto& rect : wordBank_rects)
	{
		cv::Mat im (image, rect);
		ocr->SetImage(im.data, im.cols, im.rows, 3, im.step);
		ocr->SetSourceResolution(70);
		wordBank.add(ocr->GetUTF8Text());
	}

	//Recognize letters
	int lastY = image.cols;
	ocr->SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
	for (auto& rect : letter_rects)
	{
		if (rect.y < (lastY - 20)) ++letterRows;
		++letterCount;
		lastY = rect.y;

		rect.y -= 5;
		rect.x -= 5;
		rect.width += 10;
		rect.height += 10;

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
		letters.add(ocr->GetUTF8Text());

		// cv::namedWindow("Puzzle", cv::WINDOW_AUTOSIZE);
		// cv::imshow("Puzzle", im);
		// letters[letters.length() - 1].trim().writeln(stdout);
		// cv::waitKey(5);
	}

	ocr->End();


	//generate puzzle stream
	int letterCols = (letterCount / letterRows) + 1;
	zstring letterPool;
	zstring(letterCols).writeln(stdout);
	for (int i=letters.length()-1; i>=0; --i)
	{
		//massage letters to be alphanumeric
		auto letter = letters[i].trim().upper().at(0);
		if (letter == '|') letter = 'I';

		letterPool.append(letter);
		if (!(i % letterCols)) letterPool.append('\n');
	}

	//Make the string readable by wordSearch class
	z::core::memoryStream inFile(letterPool.wstring(), letterPool.length());
	inFile.setFormat(z::utf32);


	//word search grid
	wordSearch grid;
	if (!grid.load(inFile))
	{
		(zstring("Failed to read word search: \n") + letterPool).writeln(stdout);
		return 2;
	}

	int selNum = -1;
	//Prompt loop
	grid.print(stdout);
	z::system::stdin stdin;
	while (!stdin.empty())
	{
		zstring line;
		line.readln(stdin);
		++selNum;
		if (selNum >= wordBank.length()) selNum = 0;

		auto findWord = wordBank[selNum].trim().remove(' ').upper();

		grid.reset();
		grid.find(findWord);

		// grid.select(selNum, 12);
		//Move cursor back up to the top
		(zstring("\033[") + (grid.height()+1) + 'A').write(stdout);
		//Print results
		grid.print(stdout);
		(zstring("\033[K") + selNum + ": " + findWord).write(stdout);
	}
	zstring().writeln(stdout);

}

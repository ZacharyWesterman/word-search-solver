#include <z/file.hpp>
#include <z/system.hpp>
#include <z/core.hpp>
#include <z/util.hpp>

// #include "wordSearch.hpp"

#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect.hpp>

//for letting this thread sleep
#include <chrono>
#include <thread>

int main(int argc, char** argv)
{
	z::system::stderr stderr;
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
	// cv::Mat connected;
	// morphKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9, 1));
	// cv::morphologyEx(bw, connected, cv::MORPH_CLOSE, morphKernel);


	// find contours
	cv::Mat mask = cv::Mat::zeros(bw.size(), CV_8UC1);
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(bw, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));


	// filter contours
	std::vector<cv::Rect> letters;
	for(int idx = 0; idx >= 0; idx = hierarchy[idx][0])
	{
		cv::Rect rect = cv::boundingRect(contours[idx]);
		cv::Mat maskROI(mask, rect);
		maskROI = cv::Scalar(0, 0, 0);
		// fill the contour
		cv::drawContours(mask, contours, idx, cv::Scalar(255, 255, 255), CV_FILLED);
		// ratio of non-zero pixels in the filled region
		// double r = (double)cv::countNonZero(maskROI)/(rect.width*rect.height);

		auto volume = rect.height * rect.width;
		if ((volume > 10) && (volume < 10000))
		{
			cv::Rect box;

			int cx = rect.x + (rect.width >> 1);
			int cy = rect.y + (rect.height >> 1);

			box.x = cx - 15;
			box.y = cy - 15;
			box.width = 30;
			box.height = 30;

			if (box.x > rect.x) box.x = rect.x;
			if (box.y > rect.y) box.y = rect.y;
			if (rect.width > box.width) box.width = rect.width;
			if (rect.height > box.height) box.height = rect.height;

			letters.push_back(box);

			// cv::rectangle(image, rect, cv::Scalar(0, 255, 0), 2);
		}
	}

	cv::groupRectangles(letters, 1,0.01);
	for (auto& rect : letters)
	{
		cv::rectangle(image, rect, cv::Scalar(0, 255, 0), 2);
	}

	//Open window
	const char winName[] = "Display Window";
	cv::namedWindow(winName, cv::WINDOW_AUTOSIZE);
	cv::imshow(winName, image);
	cv::waitKey(0);

	return 0;

	/*

	//Load dictionary of words.
	// zpath dictPath = "/usr/share/dict/words";
	// if (!z::file::exists(dictPath)) dictPath = "data/US_dict.txt";
	zpath dictPath = "data/US_dict.txt";
	z::file::inputStream inFile (dictPath);
	z::util::dictionary dict;

	if (dict.read(inFile) < 0)
	{
		(zstring("Unable to load dictionary file ") + dictPath).writeln(console);
		return 1;
	}

	inFile.close();
	(zstring("Loaded ") + dict.wordCount() + " words from " + dictPath).writeln(console);

	//Load word search grid from file
	zpath filePath = "examples/example1.txt";
	inFile.open(filePath);

	//word search grid
	wordSearch grid;
	if (!grid.load(inFile))
	{
		(zstring("Unable to read input file ") + filePath).writeln(console);
		return 2;
	}

	z::core::timer timer;
	for (int i=5; i<100; ++i) grid.scan(dict,i);
	double millis = (double)timer.micros() / 1000.0;
	(zstring(100-5) + " scans took " + zstring(millis,10,3)  + "ms (avg " + zstring(millis/(100-5),10,3) + "ms)").writeln(console);

	int selNum = -1;

	//Prompt loop
	grid.print(console);
	z::core::string<> line;
	while (!console.empty())
	{
		//Clear this line and ask for next word
		// zstring("\033[KWord to find: ").write(console);
		line.readln(console);

		if (!line.length() && console.empty()) break;

		++selNum;
		grid.select(selNum, 12);
		// grid.reset();
		// grid.find(line.upper());

		//Move cursor back up to the top
		(zstring("\033[") + (grid.height()+1) + 'A').write(console);
		//Print results
		grid.print(console);
		(zstring("\033[K") + grid.getMatch(selNum)).write(console);

		if (selNum >= grid.matchCount()) selNum = -1;
	}
	zstring().writeln(console);*/

}

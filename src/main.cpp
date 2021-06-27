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
#include "cv/pointsOfInterest.hpp"
#include "cv/colors.hpp"

//Word search
// #include "shared/wordSearch.hpp"

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

	auto rects = pointsOfInterest(image);

	//draw rectangles
	int i=0;
	for (auto& rect : rects)
	{
		cv::rectangle(image, rect, color(i++), 2);
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

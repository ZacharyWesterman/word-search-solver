#include <z/file.hpp>
#include <z/system.hpp>
#include <z/core.hpp>
#include <z/util.hpp>

//Image processing
#include "cv/processImage.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

//for letting this thread sleep
#include <chrono>
#include <thread>

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
		zstring("Input image required.").writeln(stderr);
		return 1;
	}

	std::string imagePath = argv[1];
	cv::Mat image = cv::imread(imagePath, CV_LOAD_IMAGE_COLOR);
	if (!image.data)
	{
		(zstring("Unable to load ")+imagePath.c_str()).writeln(stderr);
		return 2;
	}

	wordSearch grid;
	image = processImage(image, grid, true, true);

	//Open window and display
	cv::namedWindow("Puzzle", cv::WINDOW_AUTOSIZE);
	// cv::resizeWindow("Puzzle", 800, 800);
	cv::imshow("Puzzle", image);
	cv::waitKey(0);
}

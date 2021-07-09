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
	// int i=0;
	// for (auto& rect : rects)
	// {
	// 	cv::rectangle(image, rect, color(i++), 2);
	// }

	//draw lines connecting rectangles
	auto connections = connectNearest(rects);
	"{"_u8.writeln(stdout);
	for (auto& i : connections)
	{
		"  {"_u8.write(stdout);
		for (auto k : i) (" "_u8 + k).write(stdout);
		" }"_u8.writeln(stdout);
	}
	"}"_u8.writeln(stdout);
	for (int i = 0; i < connections.length(); ++i)
	{
		// if (i != connections.length()-4) continue;
		auto& row = connections[i];

		for (int k = 0; k < row.length(); ++k)
		{
			cv::rectangle(image, rects[row[k]], color(i), 2);

			if (k)
			{
				int fromX = rects[row[k-1]].x + (rects[row[k-1]].width / 2);
				int fromY = rects[row[k-1]].y + (rects[row[k-1]].height / 2);
				int toX = rects[row[k]].x + (rects[row[k]].width / 2);
				int toY = rects[row[k]].y + (rects[row[k]].height / 2);

				cv::arrowedLine(
					image,
					cv::Point(fromX, fromY),
					cv::Point(toX, toY),
					color(i),
					2
				);
			}

			cv::putText(
				image,
				zpath(row[k]).cstring(),
				cv::Point(rects[row[k]].x, rects[row[k]].y),
				cv::FONT_HERSHEY_DUPLEX,
				1,
				color(row[k]),
				1
			);
		}
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

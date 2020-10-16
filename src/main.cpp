#include <z/file.hpp>
#include <z/system.hpp>
#include <z/core.hpp>
#include <z/util.hpp>

#include "imageProc.hpp"
#include "OCR.hpp"
#include "colors.hpp"
#include "encircle.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

//for letting this thread sleep
#include <chrono>
#include <thread>

//Word search
#include "wordSearch.hpp"

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

	z::core::timer timer;

	std::vector<cv::Rect> wordBank_rects;
	std::vector<cv::Rect> letter_rects;
	getImageRects(image, wordBank_rects, letter_rects);
	(zstring("Scanned image in ") + (timer.millis() / 1000.0) + "s").writeln(stdout);



	// int last = -1;
	// for (int i=0; i<letter_rects.size(); ++i)
	// {
	// 	auto rect = letter_rects[i];
	// 	int tot = 256 * 3;
	// 	int r(0), g(0), b(0);
	//
	// 	int pos = i * tot / letter_rects.size();
	// 	// pos *= 5;
	// 	// pos %= tot;
	//
	// 	if (pos < 256)
	// 	{
	// 		r = pos;
	// 		b = 256 - pos;
	// 	}
	// 	else if (pos < 256 * 2)
	// 	{
	// 		r = 256 * 2 - pos;
	// 		g = pos - 256;
	// 	}
	// 	else
	// 	{
	// 		g = 256 * 3 - pos;
	// 		b = pos - 256 * 2;
	// 	}
	//
	// 	cv::rectangle(image, rect, cv::Scalar(b, g, r), 2);
	//
	// 	if (last + 1 != i)
	// 	{
	// 		cv::putText(
	// 			image,
	// 			"?",
	// 			cv::Point(letter_rects[i].x + letter_rects[i].width / 2, letter_rects[i].y + letter_rects[i].height),
	// 			cv::FONT_HERSHEY_DUPLEX,
	// 			0.5,
	// 			cv::Scalar(0,0,0),
	// 			1
	// 		);
	// 	}
	// 	last ++;
	// }
	// cv::namedWindow("Puzzle", cv::WINDOW_FREERATIO);
	// cv::imshow("Puzzle", image);
	// cv::waitKey(0);
	// return 0;//TEMP

	timer.reset();
	z::core::array<z::core::string<z::utf8> > wordBank = ocrWords(image, wordBank_rects);
	(zstring("Word recognition took ") + (timer.millis() / 1000.0) + "s").writeln(stdout);

	timer.reset();
	z::core::array<z::core::string<z::utf8> > letters = ocrLetters(image, letter_rects);
	(zstring("Letter recognition took ") + (timer.millis() / 1000.0) + "s").writeln(stdout);

	int fontScale = (image.rows > 1024) ? 2 : 1;

	//generate puzzle stream
	zstring letterPool;
	int letterCols = 1;
	int maxLetterSize = 0;
	for (int i=0; i<letters.length(); ++i)
	{
		//massage letters to be alphanumeric
		// auto letter = letters[i].trim().upper().at(0);
		// if (letter == '|') letter = 'I';


		if (i && (letter_rects[i].x < letter_rects[i-1].x))
		{
			++letterCols;
			letterPool.append('\n');
		}

		// cv::putText(
		// 	image,
		// 	letters[i].cstring(),
		// 	cv::Point(letter_rects[i].x + letter_rects[i].width / 2, letter_rects[i].y + letter_rects[i].height),
		// 	cv::FONT_HERSHEY_DUPLEX,
		// 	0.25 * fontScale,
		// 	cv::Scalar(0,0,200),
		// 	1
		// );

		letterPool.append(zstring(letters[i][0]));
		int letterSize = (letter_rects[i].width + letter_rects[i].height) / 2;
		if (maxLetterSize < letterSize) maxLetterSize = letterSize;
	}
	int letterRows = (letters.length() / letterCols);

	// letterPool.writeln(stdout);
	// return 0;

	//Make the string readable by wordSearch class
	z::core::memoryStream inFile(letterPool.wstring(), letterPool.length());
	inFile.setFormat(z::utf32);


	// word search grid
	wordSearch grid;
	if (!grid.load(inFile))
	{
		(zstring("Failed to read word search: \n") + letterPool).writeln(stdout);
		return 2;
	}


	//Generate word bank overlay
	for (int i=0; i<wordBank_rects.size(); ++i)
	{
		cv::rectangle(image, wordBank_rects[i], color(i), 2);

		(zstring(i)+": " + wordBank[i]).writeln(stdout);

		// cv::putText(
		// 	image,
		// 	wordBank[i].cstring(),
		// 	cv::Point(wordBank_rects[i].x - (20 * fontScale + 5), wordBank_rects[i].y + wordBank_rects[i].height),
		// 	cv::FONT_HERSHEY_DUPLEX,
		// 	0.5 * fontScale,
		// 	CV_RGB(0,0,0),
		// 	1
		// );
	}

	//Solve and generate solution overlay
	cv::Rect r1 = letter_rects[0];
	cv::Rect r2 = letter_rects[letter_rects.size()-1];
	int xmin = r1.x + r1.width / 2;
	int ymin = r1.y + r1.height / 2;
	int xmax = r2.x + r2.width / 2;
	int ymax = r2.y + r2.height / 2;

	int xlen = xmax - xmin;
	int ylen = ymax - ymin;


	int xdiff = (xmax - xmin) / (letterRows - 1);
	int ydiff = (ymax - ymin) / (letterCols - 1);
	int thickness = 2 + (image.rows > 1024);

	grid.print(stdout);

	int located = 0;
	for (int i=0; i<wordBank.length(); ++i)
	{
		auto matches = grid.find(wordBank[i]);
		if (matches) ++located;
		else
		{
			cv::putText(
				image,
				"?",
				cv::Point(wordBank_rects[i].x - thickness * 3, wordBank_rects[i].y + (wordBank_rects[i].height / 2)),
				cv::FONT_HERSHEY_DUPLEX,
				0.5 * fontScale,
				CV_RGB(0,0,0),
				1
			);
		}

		for (int k=0; k<matches; ++k)
		{
			auto match = grid.getMatchData(k);

			int x0 = ((match.x * xlen) / (letterRows-1)) + xmin;
			int x1 = ((match.width * xlen) / (letterRows-1)) + xmin;
			int y0 = ((match.y * ylen) / (letterCols-1)) + ymin;
			int y1 = ((match.height * ylen) / (letterCols-1)) + ymin;

			encircle(
				image,
				cv::Point(x0,y0),
				cv::Point(x1,y1),
				color(i),
				thickness,
				maxLetterSize / 2
			);
		}
	}

	(zstring("Found ")+located+" of the "+wordBank.length()+" detected words.").writeln(stdout);

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
	// cv::resizeWindow("Puzzle", 800, 800);
	cv::imshow("Puzzle", image);
	cv::waitKey(0);
}

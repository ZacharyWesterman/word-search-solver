#include "processImage.hpp"

#include <z/file.hpp>
#include <z/system.hpp>
#include <z/core.hpp>
#include <z/util.hpp>

#include "getImageRects.hpp"
#include "OCR.hpp"
#include "colors.hpp"
#include "encircle.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

cv::Mat processImage(const cv::Mat& inputImage, wordSearch& grid, bool resize, bool log)
{
	z::system::stderr stderr;
	z::system::stdout stdout;

	cv::Mat image = inputImage;

	z::core::timer timer;

	std::vector<cv::Rect> wordBank_rects;
	std::vector<cv::Rect> letter_rects;
	getImageRects(image, wordBank_rects, letter_rects);
	if (log) (zstring("Scanned image in ") + (timer.millis() / 1000.0) + "s").writeln(stdout);

	timer.reset();
	z::core::array<z::core::string<z::utf8> > wordBank = ocrWords(image, wordBank_rects);
	if (log) (zstring("Word recognition took ") + (timer.millis() / 1000.0) + "s").writeln(stdout);

	timer.reset();
	z::core::array<z::core::string<z::utf8> > letters = ocrLetters(image, letter_rects);
	if (log) (zstring("Letter recognition took ") + (timer.millis() / 1000.0) + "s").writeln(stdout);

	int fontScale = (image.rows > 1024) ? 2 : 1;

	//generate puzzle stream
	zstring letterPool;
	int letterCols = 1;
	int maxLetterSize = 0;
	for (int i=0; i<letters.length(); ++i)
	{
		if (i && (letter_rects[i].x < letter_rects[i-1].x))
		{
			++letterCols;
			letterPool.append('\n');
		}

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
	if (!grid.load(inFile))
	{
		if (log) (zstring("Failed to read word search: \n") + letterPool).writeln(stdout);
		return inputImage;
	}


	//Generate word bank overlay
	for (int i=0; i<wordBank_rects.size(); ++i)
	{
		cv::rectangle(image, wordBank_rects[i], color(i), 2);

		if (log) (zstring(i)+": " + wordBank[i]).writeln(stdout);

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

	if (log) (zstring("Found ")+located+" of the "+wordBank.length()+" detected words.").writeln(stdout);

	if (resize)
	{
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
	}

	return image;
}

//Image processing
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect.hpp>

// float cellSize = 10.0;
// float cellOffset = 0.0;

static bool rectContains(const cv::Rect& rect1, const cv::Rect& rect2) noexcept
{
	bool xmin = (rect1.x <= rect2.x);
	bool ymin = (rect1.y <= rect2.y);
	bool xmax = (rect1.x + rect1.width) >= (rect2.x + rect2.width);
	bool ymax = (rect1.y + rect1.height) >= (rect2.y + rect2.height);

	return xmin && ymin && xmax && ymax;
}

// static bool rectCompare(const cv::Rect& l, const cv::Rect& r) noexcept
// {
// 	cv::Point l_c (l.x + l.width / 2, l.y + l.height / 2);
// 	cv::Point r_c (r.x + r.width / 2, r.y + r.height / 2);
// 	float cellSize = l.height / 2;
//
// 	// if (l.height < 100)
// 	{
// 		cellSize = 50;
// 	}
//
// 	float lCell = floorf(l_c.y / cellSize);
// 	float rCell = floorf(r_c.y / cellSize);
// 	if(lCell == rCell) return l_c.x > r_c.x;
// 	return (lCell > rCell);
// }

static bool rectCompare_r(const cv::Rect& l, const cv::Rect& r) noexcept
{
	// return l.x * l.y < r.x * r.y;

	cv::Point l_c (l.x + l.width / 2, l.y + l.height / 2);
	cv::Point r_c (r.x + r.width / 2, r.y + r.height / 2);

	//
	float cellSize = l.height * 2 / 3;
	//
	// // if (l.height < 30)
	// // {
	// // 	cellSize = 10;
	// // }
	//
	float lCell = roundf(l.y / cellSize);
	float rCell = roundf(r.y / cellSize);

	if (abs(l.y - r.y) > abs(l.x - r.x)) return l.y < r.y;
	// return l_c.x < r_c.x;

	if(lCell == rCell) return l.x < r.x;
	return (lCell < rCell);
}

// static void sortLetters(std::vector<cv::Rect>& letters) noexcept
// {
//
// }

cv::Rect increase(cv::Rect rect, int amount) noexcept
{
	rect.x -= amount;
	rect.y -= amount;
	rect.width += amount << 1;
	rect.height += amount << 1;

	return rect;
}

void getImageRects(cv::Mat& image, std::vector<cv::Rect>& wordBank, std::vector<cv::Rect>& letters) noexcept
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
		else if (volume > 10000) bounds = rect;
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
			//filter out rects that are contained by other rects
			bool filter = false;
			int i = 0;
			for (int i=0; i<interestPoints2.size(); ++i)
			{
				if (rectContains(interestPoints2[i], rect))
				{
					filter = true;
				}
				else if (rectContains(rect,interestPoints2[i]))
				{
					interestPoints2.erase(interestPoints2.begin()+i);
					--i;
				}
			}

			if (!filter) interestPoints2.push_back(rect);
		}
	}


	// cv::rectangle(image, bounds, cv::Scalar(0,255,0),2);

	//letters that are outside the determined bounds are part of the word bank

	for (auto& rect : interestPoints)
	{
		if (rectContains(bounds, rect))
		{
			letters.push_back(increase(rect,5));
			// cv::rectangle(image, rect, cv::Scalar(0, 255, 0), 2);
		}
	}


	//organize word bank by which side they're on
	int count[4] = {0};
	std::vector<cv::Rect> wbankList[4];
	for (auto& rect : interestPoints2)
	{
		if (!rectContains(bounds, rect))
		{
			auto r = increase(rect,5);

			if (rect.x < bounds.x)
			{
				wbankList[0].push_back(r);
				++count[0];
			}
			if (rect.x > (bounds.x + bounds.width))
			{
				wbankList[1].push_back(r);
				++count[1];
			}
			if (rect.y < bounds.y)
			{
				wbankList[2].push_back(r);
				++count[2];
			}
			if (rect.y > (bounds.y + bounds.height))
			{
				wbankList[3].push_back(r);
				++count[3];
			}
			// wordBank.push_back(increase(rect,5));
			// cv::rectangle(image, rect, cv::Scalar(0, 155, 100), 2);
		}
	}

	//the word bank is on the side with the most words
	int max = 0;
	for (int i=1; i<4; ++i)
	{
		if (count[i] > count[max]) max = i;
	}
	wordBank = wbankList[max];

	// cv::rectangle(image, bounds, cv::Scalar(255, 0, 0), 2);

	//sort rectangles by position
	std::sort(letters.begin(), letters.end(), rectCompare_r);
	std::sort(wordBank.begin(), wordBank.end(), rectCompare_r);

	//merge any word bank items that are touching horizontally
	for (int i=wordBank.size()-1; i>0; --i)
	{
		if (((wordBank[i-1].x + wordBank[i-1].width) > wordBank[i].x) && (wordBank[i-1].x < wordBank[i].x))
		{
			wordBank[i-1].width = (wordBank[i].x + wordBank[i].width) - wordBank[i-1].x;
			wordBank[i-1].height = (wordBank[i].y + wordBank[i].height) - wordBank[i-1].y;
			wordBank.erase(wordBank.begin() + i, wordBank.begin() + i + 1);
		}
	}
}

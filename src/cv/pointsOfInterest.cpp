#include "pointsOfInterest.hpp"

#include <opencv2/imgproc/imgproc.hpp>
#include "rectContains.hpp"

z::core::array<cv::Rect> pointsOfInterest(const cv::Mat& image)
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

	//find connected contours
	std::vector<std::vector<cv::Point>> contours_connected;
	std::vector<cv::Vec4i> hierarchy2;
	cv::findContours(connected, contours_connected, hierarchy2, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	z::core::array<cv::Rect> rects;
	for(int idx = 0; idx >= 0; idx = hierarchy[idx][0])
	{
		cv::Rect rect = cv::boundingRect(contours[idx]);
		cv::Mat maskROI(mask, rect);
		maskROI = cv::Scalar(0, 0, 0);
		// fill the contour
		cv::drawContours(mask, contours, idx, cv::Scalar(255, 255, 255), CV_FILLED);

		bool filterOut = false;
		int factor = 4;
		for (int i=0; i<rects.length(); ++i)
		{

			//if this rect fully contains a previous rect
			//and this rects volume is less than 4x the previous one's,
			//then delete that previous rect
			if (rectContains(rect, rects[i]))
			{
				if ((rect.height < (rects[i].height * factor)) || (rect.width < (rects[i].width * factor)))
				{
					rects.remove(i);
					--i;
				}
			}
			//if a previous rect fully contains this rect
			//and the previous rect's volume is less than 4x of this one's,
			//then don't add this one.
			else if (rectContains(rects[i], rect))
			{
				if ((rects[i].height < (rect.height * factor)) || (rects[i].width < (rect.width * factor)))
				{
					filterOut = true;
					break;
				}
			}
		}

		if (!filterOut) rects.add(rect);
	}

	return rects;
}

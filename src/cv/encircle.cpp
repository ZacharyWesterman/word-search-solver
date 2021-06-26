#include "encircle.hpp"
#include <math.h>
#include <z/math/constants.hpp>
#include <string>

double dist(const cv::Point& p1, const cv::Point& p2) noexcept
{
	cv::Point diff = p1 - p2;
	return cv::sqrt(diff.x*diff.x + diff.y*diff.y);
}

void encircle(cv::Mat& image, const cv::Point& p1, const cv::Point& p2, const cv::Scalar& color, int thickness, int radius) noexcept
{
	//what angle is the line at
	double angle = std::atan2(p1.y-p2.y,p1.x-p2.x);

	double xoff = sin(angle) * radius;
	double yoff = cos(angle) * radius;

	//"top" side
	cv::line(
		image,
		cv::Point(p1.x-xoff, p1.y+yoff),
		cv::Point(p2.x-xoff, p2.y+yoff),
		color,
		thickness
	);

	//"bottom" side
	cv::line(
		image,
		cv::Point(p1.x+xoff, p1.y-yoff),
		cv::Point(p2.x+xoff, p2.y-yoff),
		color,
		thickness
	);


	angle *= 180.0 / z::math::pi;

	//curve at beginning
	cv::ellipse(
		image,
		p1,
		cv::Size(radius,radius),
		angle,
		-90,
		90,
		color,
		thickness
	);

	//curve at end
	cv::ellipse(
		image,
		p2,
		cv::Size(radius,radius),
		angle+180,
		-90,
		90,
		color,
		thickness
	);
}

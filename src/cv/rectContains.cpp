#include "rectContains.hpp"

bool rectContains(const cv::Rect& rect1, const cv::Rect& rect2) noexcept
{
	bool xmin = (rect1.x <= rect2.x);
	bool ymin = (rect1.y <= rect2.y);
	bool xmax = (rect1.x + rect1.width) >= (rect2.x + rect2.width);
	bool ymax = (rect1.y + rect1.height) >= (rect2.y + rect2.height);

	return xmin && ymin && xmax && ymax;
}

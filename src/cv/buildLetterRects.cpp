#include "buildLetterRects.hpp"

z::core::array<z::core::array<cv::Rect>> buildLetterRects(const z::core::array<cv::Rect>& rects, const z::core::array<z::core::array<int>>& connections)
{
	z::core::array<z::core::array<cv::Rect>> letterRects;

	for (const auto& row : connections)
	{
		z::core::array<cv::Rect> rectList;
		for (int i : row)
		{
			//add rect, but increase size by 25%
			cv::Size deltaSize (rects[i].width * 0.25f, rects[i].height * 0.25f);
			cv::Point offset (deltaSize.width/2, deltaSize.height/2);
			rectList.add(rects[i] + deltaSize - offset);
		}
		letterRects.add(rectList);
	}

	return letterRects;
}

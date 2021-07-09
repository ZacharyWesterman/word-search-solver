#include "buildWordRects.hpp"
#include "avgMaxMin.hpp"

z::core::array<cv::Rect> buildWordRects(const z::core::array<cv::Rect>& rects, const z::core::array<z::core::array<int>>& connections)
{
	z::core::array<cv::Rect> wordRects;

	for (const auto& row : connections)
	{
		cv::Rect rect = rects[row[0]];

		int average, min, max;
		avgMaxMin(rects, row, average, min, max);
		for (int i = 1; i<row.length(); ++i)
		{
			const auto& A = rects[row[i-1]];
			const auto& B = rects[row[i]];
			const int dist = B.x - (A.x + A.width);

			//make new word
			if (dist > (average * 3)/2)
			{
				//add rect, but increase size by 25%
				cv::Size deltaSize (rect.width * 0.f, rect.height * 0.25f);
				cv::Point offset (deltaSize.width/2, deltaSize.height/2);
				wordRects.add(rect + deltaSize - offset);

				rect = rects[row[i]]; //start new word
			}
			else //expand existing word (union of both rects)
			{
				rect = rect | rects[row[i]];
			}
		}

		//add rect, but increase size by 25%
		cv::Size deltaSize (rect.width * 0.f, rect.height * 0.25f);
		cv::Point offset (deltaSize.width/2, deltaSize.height/2);
		wordRects.add(rect + deltaSize - offset);
	}

	return wordRects;
}

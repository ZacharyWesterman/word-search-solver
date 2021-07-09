#include "groupBySpread.hpp"
#include "avgMaxMin.hpp"

z::core::array<z::core::array<int>> groupBySpread(const z::core::array<cv::Rect>& rects, const z::core::array<z::core::array<int>>& connections, bool significant)
{
	z::core::array<z::core::array<int>> result;

	for (const auto& row : connections)
	{
		int average, min, max;
		if (avgMaxMin(rects, row, average, min, max) == significant)
		{
			result.add(row);
		}
	}

	return result;
}

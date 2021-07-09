#include "sortVertically.hpp"

z::core::array<z::core::array<int>> sortVertically(const z::core::array<cv::Rect>& rects, const z::core::array<z::core::array<int>>& connections)
{
	z::core::array<z::core::array<int>> newConns;

	for (const auto& row : connections)
	{
		int insertPos = 0;
		for (int i=0; i<newConns.length(); ++i)
		{
			if (rects[newConns[i][0]].y < rects[row[0]].y) insertPos = i + 1;
		}
		newConns.insert(row, insertPos);
	}

	return newConns;
}

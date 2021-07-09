#include "avgMaxMin.hpp"

bool avgMaxMin(const z::core::array<cv::Rect>& rects, const z::core::array<int>& connections, int& average, int& min, int& max)
{
	if (connections.length() < 2) return false;

	min = 0;
	max = 0;
	average = 0;

	for (int i=1; i<connections.length(); ++i)
	{
		const auto& A = rects[connections[i-1]];
		const auto& B = rects[connections[i]];

		const int dist = B.x - (A.x + A.width);
		if (i == 1) min = max = dist;
		else if (dist < min) min = dist;
		else if (dist > max) max = dist;

		average += dist;
	}

	average /= connections.length();
	return (min * 3) >= max; //if there's less than a tripling between min & max, then difference is negligible.
}

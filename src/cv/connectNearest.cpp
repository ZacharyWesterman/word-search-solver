#include "connectNearest.hpp"

z::core::array<z::core::array<int>> connectNearest(const z::core::array<cv::Rect>& rects)
{
	z::core::array<z::core::array<int>> connections;

	for (int A = 0; A < rects.length(); ++A)
	{
		z::core::array<int> conn;

		//connect rects A & B whose respective +x and -x borders are closest,
		//and are within 1 unit of rect A's height.
		int closestX = -1;
		for (int B = 0; B < rects.length(); ++B)
		{
			if (B == A) continue;

			int newDist = rects[B].x - (rects[A].x + rects[A].width);
			if (newDist < 0) continue; //don't connect if B is to the left of A.

			//don't connect if B's -y border is above A's +y border.
			if (rects[B].y > (rects[A].y + rects[A].height)) continue;
			//don't connect if B's +y border is below A's -y border.
			if ((rects[B].y + rects[B].height) < rects[A].y) continue;

			if (closestX < 0)
			{
				closestX = B;
				continue;
			}

			int oldDist = rects[closestX].x - (rects[A].x + rects[A].width);
			if (newDist < oldDist)
			{
				closestX = B;
			}
		}

		//connect rects A & B whose respective +x and -x borders are closest.
		if (closestX > -1) conn.add(closestX);

		connections.add(conn);
	}

	return connections;
}

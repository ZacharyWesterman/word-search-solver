#include "connectNearest.hpp"
#include "rectContains.hpp"

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

			//don't connect if A fully contains B, or if B fully contains A.
			if (rectContains(rects[A], rects[B]) || rectContains(rects[B], rects[A])) continue;

			int newDist = rects[B].x - (rects[A].x);
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

			int oldDist = rects[closestX].x - (rects[A].x);
			if (newDist < oldDist)
			{
				closestX = B;
			}
		}

		//connect rects A & B whose respective +x and -x borders are closest.
		//keep them in an ordered list
		if (closestX > -1)
		{
			int row[] = {-1, -1};
			int col[] = {-1, -1};
			for (int i=0; i<connections.length(); ++i)
			{
				auto& item = connections[i];

				if ((row[0] == -1) && ((col[0] = item.find(A)) > -1))
				{
					row[0] = i;
				}
				else if ((row[1] == -1) && ((col[1] = item.find(closestX)) > -1))
				{
					row[1] = i;
				}
			}


			if (row[0] > -1)
			{
				connections[row[0]].insert(closestX, col[0] + 1);
			}
			else if (row[1] > -1)
			{
				connections[row[1]].insert(A, col[1]);
			}
			else
			{
				connections.add(z::core::array<int>{A, closestX});
			}

			if ((row[0] > -1) && (row[1] > -1))
			{
				connections[row[0]].replace(col[0]+1, 1, connections[row[1]]);
				connections.remove(row[1]);
			}
		}

	}

	return connections;
}

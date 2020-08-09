#include "wordSearch.hpp"

#include <z/core/array.hpp>

wordSearch::wordSearch()
{
	data = nullptr;
	highlighted = nullptr;
	dataWidth = 0;
	dataHeight = 0;
}

wordSearch::~wordSearch()
{
	clear();
}

bool wordSearch::load(z::core::inputStream& input)
{
	clear();

	//Read word grid from input stream
	z::core::array< z::core::string<> > letterGrid;

	z::core::string<> line;
	while (input.good() && !input.empty())
	{
		line.readln(input);
		if (!line.length()) break;
		letterGrid.add(line);
	}

	//verify input, fail if not an X*Y rectangle of characters
	if (!letterGrid.length()) return false;
	int len = letterGrid[0].length();
	if (!len) return false;
	for (int i=1; i<letterGrid.length(); ++i)
	{
		if (letterGrid[i].length() != len) return false;
	}

	//initialize data
	dataWidth = len;
	dataHeight = letterGrid.length();

	data = new uint32_t[dataWidth * dataHeight];
	highlighted = new int[dataWidth * dataHeight];

	for (int y=0; y<dataHeight; ++y)
	{
		for (int x=0; x<dataWidth; ++x)
		{
			data[y*dataWidth + x] = letterGrid[y][x];
			highlighted[y*dataWidth + x] = 0;
		}
	}

	return true;
}

bool wordSearch::loaded() const
{
	return (bool)data;
}

void wordSearch::clear()
{
	matches.clear();

	if (data) delete[] data;
	if (highlighted) delete[] highlighted;

	data = nullptr;
	highlighted = nullptr;
	dataWidth = 0;
	dataHeight = 0;
}

void wordSearch::reset()
{
	matches.clear();

	if (!highlighted) return;

	for (int y=0; y<dataHeight; ++y)
	{
		for (int x=0; x<dataWidth; ++x)
		{
			highlighted[y*dataWidth + x] = 0;
			oldColor = 0;
		}
	}
}

int wordSearch::width() const
{
	return dataWidth;
}

int wordSearch::height() const
{
	return dataHeight;
}

int wordSearch::matchCount() const
{
	return matches.length();
}

bool wordSearch::select(int index, int color)
{
	if (index == selectedMatch) return true;

	//reset old match
	if (selectedMatch < matches.length())
	{
		auto match = matches[selectedMatch];
		setHighlight(match.pos, match.dir, match.len, oldColor);
	}

	if ((index >= matches.length()) || (index < 0)) return false;

	auto match = matches[index];
	oldColor = highlighted[match.pos];
	setHighlight(match.pos, match.dir, match.len, color);

	selectedMatch = index;
	return true;
}

z::core::string<> wordSearch::getMatch(int index) const
{
	z::core::string<> thisMatch;
	if ((index < matches.length()) && (index >= 0))
	{
		auto match = matches[index];
		for (int i=0; i<match.len; ++i)
		{
			thisMatch += data[match.pos + match.dir*i];
		}
	}

	return thisMatch;
}

void wordSearch::print(z::core::outputStream& output) const
{
	for (int y=0; y<dataHeight; ++y)
	{
		int active = 0;

		for (int x=0; x<dataWidth; ++x)
		{
			int thisHighlight = highlighted[y*dataWidth + x];

			if (thisHighlight && active && (active != thisHighlight))
			{
				zstring("\033[0m").write(output);
				active = 0;
			}

			if (thisHighlight && !active)
			{
				(zstring("\033[48;5;")+thisHighlight+'m').write(output);
				active = thisHighlight;
			}

			if (!thisHighlight && active)
			{
				zstring("\033[0m").write(output);
				active = 0;
			}
			zstring(" ").write(output);

			zstring(data[y*dataWidth + x]).write(output);
		}
		if (active)
			zstring("\033[0m").writeln(output);
		else
			zstring().writeln(output);
	}
}

int wordSearch::find(const z::core::string<>& text, int occurrence, int color)
{
	if (!text.length()) return 0; //null string never matches any

	const int directions[8] = {
		-dataWidth - 1,
		-1,
		dataWidth - 1,
		dataWidth,
		-dataWidth,
		-dataWidth + 1,
		1,
		dataWidth + 1,
	};

	int matchCount = 0;

	for (int i=0; i<dataWidth*dataHeight; ++i)
	{
		if (z::core::toUpper(data[i]) != z::core::toUpper(text[0])) continue;

		//if character matches the first letter, check surrounding letters
		if (text.length() > 1)
		{
			for (int j=0; j<8; ++j) //go in circle
			{
				int d = directions[j];

				//check X boundary
				int xdir = (j < 3)*-1 + (j > 5);
				int xend = (i % dataWidth) + (xdir * (text.length() - 1));
				if ((xend < 0) || (xend >= dataWidth)) continue;

				bool matched = true;
				//match as many characters as possible
				for (int k=1; k<text.length(); ++k)
				{
					if (z::core::toUpper(data[i+d*k]) != z::core::toUpper(text[k]))
					{
						matched = false;
						break;
					}
				}
				if (matched)
				{
					++matchCount;
					setHighlight(i,d,text.length(),color);
				}
			}
		}
		else
		{
			++matchCount;
			setHighlight(i,0,1,color);
		}
	}

	return matchCount;
}

void wordSearch::setHighlight(int pos, int dir, int len, int color)
{
	for (int i=0; i<len; ++i)
	{
		if ((pos < 0) || (pos >= dataWidth*dataHeight)) break;

		highlighted[pos] = color;
		pos += dir;
	}
}

int wordSearch::scan(const z::util::dictionary& dict, int length, int color)
{
	const int dirs[8][2] = {
		{-1,-1},
		{-1, 0},
		{-1, 1},
		{ 0, 1},
		{ 1, 1},
		{ 1, 0},
		{ 1,-1},
		{ 0,-1},
	};
	const int directions[8] = {
		-dataWidth - 1,
		-1,
		dataWidth - 1,
		dataWidth,
		dataWidth + 1,
		1,
		-dataWidth + 1,
		-dataWidth,
	};

	int matchCount = 0;

	for (int y=0; y<dataHeight; ++y)
	{
		for (int x=0; x<dataWidth; ++x)
		{
			auto rangeMain = dict.range();
			int pos = y*dataWidth + x;
			if (!dict.narrow(&rangeMain, data[pos])) continue;

			for (int j=0; j<8; ++j) //go in circle
			{
				//shortcut, require the minimum length before we begin to check.
				int xend = x + dirs[j][0] * (length - 1);
				int yend = y + dirs[j][1] * (length - 1);
				if ((xend >= dataWidth) || (xend < 0)) continue;
				if ((yend >= dataHeight) || (yend < 0)) continue;

				//narrow down words to those of minimum required length
				auto range = rangeMain;
				xend = x;
				yend = y;
				for (int i=1; i<length; ++i)
				{
					xend += dirs[j][0];
					yend += dirs[j][1];
					if (!dict.narrow(&range, data[yend*dataWidth + xend])) break;
				}
				if (range.exhausted || !range.isWord) continue; //not a word that matches the minimum, look in next direction.

				matchType thisMatch = {pos, directions[j], length};

				//TEMP: print the match
				// for (int xx=0; xx<length; ++xx)
				// {
				// 	std::cout << (char)data[pos + (directions[j] * xx)];
				// }
				// std::cout << std::endl;

				matches.add(thisMatch);
				setHighlight(pos, directions[j], length, color);
			}
		}
	}

	return (selectedMatch = matches.length());
}

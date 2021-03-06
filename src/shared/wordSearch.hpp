#pragma once

#include <z/core/stream.hpp>
#include <z/core/string.hpp>
#include <z/util/dictionary.hpp>
#include <z/core/array.hpp>

struct wordMatch
{
	int x;
	int y;
	int width;
	int height;
	int direction;
	z::core::string<> text;
};

class wordSearch
{
private:
	uint32_t* data;
	int* highlighted;
	int dataWidth;
	int dataHeight;

	struct matchType
	{
		int pos;
		int dir;
		int len;
	};

	z::core::array<matchType> matches;
	int selectedMatch;
	int oldColor;

	void setHighlight(int pos, int dir, int len, int color);

public:
	wordSearch();
	~wordSearch();

	bool load(z::core::inputStream& input);
	bool load(const zstring& input);
	bool loaded() const;

	void clear();
	void reset();

	int width() const;
	int height() const;

	int matchCount() const;

	bool select(int index, int color);
	z::core::string<> getMatch(int index) const;

	wordMatch getMatchData(int index) const;

	void print(z::core::outputStream& output) const;


	int find(const z::core::string<>& text, int color = 8);

	int scan(const z::util::dictionary& dict, int minLength, int maxLength = 0, int color = 8);
};

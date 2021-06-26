#include <z/all.hpp>
#include <cereal/archives/binary.hpp>

#include "shared/wordSearch.hpp"

int main(int argc, char** argv)
{
	z::system::stderr stderr;
	z::system::stdout stdout;
	z::system::stdin stdin;

	wordSearch search;

	if (argc >= 2)
	{
		zpath fname = argv[1];
		if (!z::file::exists(fname))
		{
			"Error: Input file does not exist or unable to stat."_u8.writeln(stderr);
			return 1;
		}

		z::file::inputStream input (fname);
		search.load(input);
	}
	else
	{
		search.load(stdin);
	}

	auto dictName = z::file::dirname(z::file::executable()) + "/data/us.dict";
	if (!z::file::exists(dictName))
	{
		("Error: Unable to load required dictionary file "_u8 + dictName).writeln(stderr);
		return 2;
	}

	std::ifstream dictFile (dictName.cstring());

	z::util::dictionary dict;
	{
		cereal::BinaryInputArchive archive (dictFile);
		archive(dict);
	}

	("Size: "_u8 + search.width() + 'x' + search.height()).writeln(stdout);
	search.print(stdout);

	z::core::array<zstring> matches;

	int matchCount = search.scan(dict, 4);
	for (int i = 0; i < matchCount; ++i)
	{
		auto data = search.getMatchData(i);

		//make sure this is the longest match
		bool longest = true;
		for (int k = i+1; k < matchCount; ++k)
		{
			auto other = search.getMatchData(k);
			if (
				other.text.beginsWith(data.text) &&
				(other.direction == data.direction) &&
				(other.x == data.x) &&
				(other.y == data.y)
			)
			{
				longest = false;
				break;
			}
		}
		if (!longest) continue;

		//Only list unique instances
		if (matches.find(data.text) <= -1)
		{
			matches.add(data.text);
			(data.text + ' ' + data.x + ' ' + data.y + ' ' + data.width + ' ' + data.height).writeln(stdout);
		}
	}

	return 0;
}

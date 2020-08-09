#include <z/file.hpp>
#include <z/system.hpp>
#include <z/core.hpp>
#include <z/util.hpp>

#include "wordSearch.hpp"


int main(int argc, char** argv)
{
	z::system::console console;
	(void)argc;
	(void)argv;

	//Load dictionary of words.
	zstring dictPath = "/usr/share/dict/words";
	z::file::inputStream inFile (dictPath);
	z::util::dictionary dict;

	if (dict.read(inFile) < 0)
	{
		(zstring("Unable to load dictionary file ") + dictPath).writeln(console);
		return 1;
	}

	inFile.close();
	(zstring("Loaded dictionary of ") + dict.wordCount() + " words.").writeln(console);

	//Load word search grid from file
	zstring filePath = "examples/example1.txt";
	inFile.open(filePath);

	//word search grid
	wordSearch grid;
	if (!grid.load(inFile))
	{
		(zstring("Unable to read input file ") + filePath).writeln(console);
		return 2;
	}

	for (int i=7; i<15; ++i) grid.scan(dict,i);
	for (int i=0; i<grid.matchCount(); ++i)
	{
		grid.select(i,4);
		grid.print(console);
		zstring().writeln(console);
	}

	/*
	//Prompt loop
	grid.print(console);
	z::core::string<> line;
	while (!console.empty())
	{
		//Clear this line and ask for next word
		zstring("\033[KWord to find: ").write(console);
		line.readln(console);

		if (!line.length()) continue;

		grid.reset();
		grid.find(line.upper());

		//Move cursor back up to the top
		(zstring("\033[") + (grid.height()+1) + 'A').write(console);
		//Print results
		grid.print(console);
	}
	zstring().writeln(console);
	*/
}

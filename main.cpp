#include <z/file.hpp>
#include <z/system.hpp>
#include <z/core.hpp>
#include <z/util.hpp>

int main(int argc, char** argv)
{
	z::system::console console;
	(void)argc;
	(void)argv;

	//Load dictionary of words.
	zstring dictPath = "/usr/share/dict/words";
	z::file::inputStream dictFile (dictPath);
	z::util::dictionary dict;

	if (dict.read(dictFile) < 0)
	{
		(zstring("Unable to load dictionary file ") + dictPath).writeln(console);
		return 1;
	}

	zstring("Dictionary loaded.").writeln(console);

	//Load word search file
	zstring filePath = "examples/example1.txt";


}

#include <z/util/dictionary.hpp>
#include <z/file/inputStream.hpp>
#include <cereal/archives/binary.hpp>

int main(int argc, char** argv)
{
	if (argc < 2) return -1;

	z::util::dictionary dict;
	z::file::inputStream in (argv[1]);
	dict.read(in, -1, true);

	std::ofstream out ("data/us.dict");
	cereal::BinaryOutputArchive archive(out);

	archive(dict);
}

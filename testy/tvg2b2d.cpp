// 
// generate fiddle output for tiny vector graphic
// specify file name on the command line
//

#include "filestream.h"
#include "tvg2blend2d.h"

using namespace tvg;

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("usage: %s <filename>.tvg\n", argv[0]);
		return 1;
	}

	FileStream fs(argv[1]);

	tvg::tvgscanner scanner(fs);
	if (!scanner.isValid){
		printf("invalid file\n");
		return 1;
	}

	writeFiddler(scanner);

	return 0;
}

// 
// generate fiddle output for tiny vector graphic
// specify file name on the command line
//

#include "filestream.h"
#include "tvg2blend2d.h"


int main(int argc, char **argv)
{
	FileStream fs(filename);

	tvg::tvgscanner scanner(fs);
	if (!scanner.isValid)
		return -1;

	writeFiddler(scanner);
}

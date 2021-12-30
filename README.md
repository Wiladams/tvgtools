# tvgtools
 Tiny Vector Graphics Tools

The file tvgscan.h is the scanner for .tvg files.  I will repeatedly
generate the tokens read from a binary .tvg file.

Typical usage
-------------

```C
// 
// generate fiddle output for tiny vector graphic
// specify file name on the command line
//

#include "filestream.h"

using namespace tvg;

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("usage: %s <filename>.tvg\n", argv[0]);
		return 1;
	}

	FileStream fs(argv[1]);

    // create a scanner on the stream of bytes
	tvg::tvgscanner scanner(fs);

    // pull tokens from the scanner and
    // do something with them
    while (true)
	{
		tvg_command_t cmd;
		if (!scanner.next(cmd))
			break;

		writeCommand(cmd);
	}

	return 0;
}
```

Essentially, create a scanner, feeding it a stream of bytes of some sort.

The tvg_command_t structure contains information for a path command (stroke, fill, fillandstroke).  Within that structure, you will find a std::vector<tvg_contour_command_t>.

```C
	struct tvg_command_t
	{
		int command=0;
		int segment_count=0;
		tvg_style_t fillStyle;
		tvg_style_t lineStyle;
		float lineWidth=0.0;

		// we want the whole command to be represented
		// in contours.  An application level thing can 
		// do what it wants with this contour information
		std::vector<tvg_contour_command_t> contours;
	};
```
The tvg_contour_command_t contains a part of a path command, such as moveTo, lineTo, arcTo, etc.  These individual commands are formed into something to be filled or stroked.

And that's about it.  If you're doing something like rendering, then you need to match the commands to your intended rendering engine, which should have a path rendering capability.

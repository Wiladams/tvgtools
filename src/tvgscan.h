#pragma once

//
// https://tinyvg.tech/download/specification.pdf
//
//
// Note: The .tvg file format is little-endian
//
// Description: The general idea with the code here is to read the 
// tinyvg file format, and output a data structure stream
// that is ready for other purposes.
//
// The only dependency other than standard libraries, is the binstream
// object.  That is to ease memory reading.  There are three routines 
// that must be implemented:
//	uint8_t		readOctet();
//	uint16_t	readUInt16();
//  uint32_t	readUInt32();
//	float		readFloat();
//  void		readBytes(void *, uint32_t);
//
//  tinyvg specifies 'little-endian' so these routines must be able to read a stream
//  of bytes and return these integer values in little-endian order (low order byte first)
//

#include <cstdint>
#include <memory>
#include <vector>

#include "binreader.h"

namespace tvg
{

	// This enumeration corresponds to the top level
	// commands from the tinyvg specification
	// do NOT change them unless that spec changes
	enum Commands {
		TVG_ERROR = -1,
		EndOfDocument = 0,		// end of the file
		FillPolygon = 1,		// fills an N-gon
		FillRectangles = 2,		// fills a set of rectangles
		FillPath = 3,		// fill a free-form path
		DrawLines = 4,		// draw a set of independent lines
		DrawLineLoop = 5,		// draw the outline of a polygon
		DrawLineStrip = 6,		// draw a list of connected lines
		DrawLinePath = 7,		// draw a free-form path outline
		OutlineFillPolygon = 8,		// combine fill and draw polygon
		OutlineFillRectangles = 9,	// combine fill and draw rectangles
		OutlineFillPath = 10,	// Combine fill and draw path
	};

	// These are geometry commands.  Mostly things are paths 
	// and contours, but lines and rects are also part of the spec
	// adding new geometry commands would go in here.
	enum GeoCommands
	{
		line = 257,
		rect = 258,
		path = 259,
	};

	// path contour building commands
	// These numbers are according to the tinyvg spec
	// don't change them.
	// You can add additional commands from the value
	// 256 and above
	enum ContourCommands
	{
		lineTo=0,
		hlineTo=1,
		vlineTo=2,
		cubicBezierTo=3,
		arcCircleTo = 4,
		arcEllipseTo = 5,
		closePath = 6,
		quadraticBezierTo=7,

		// Extensions, these are NOT part of the spec, but useful
		// for capturing contour commands in general
		moveTo=256,

	};


	// These match the TinyVG spec
	// DONT change them
	enum ColorEncoding {
		RGBA8888 = 0,
		RGB565 = 1,
		RGBAF32 = 2,
		Custom = 3,
	};

	enum CoordinateRange {
		Default = 0,	// 16-bit
		Reduced = 1,	// 8-bit
		Enhanced = 2,	// 32-bit
	};

	enum DrawingStyle {
		FlatColored = 0,
		LinearGradient = 1,
		RadialGradient = 2,
	};

	// Structures
	union tinyvg_rgba8888_t {
		uint32_t v;
		struct {
			uint8_t r;
			uint8_t g;
			uint8_t b;
			uint8_t a;
		};

		// These constructors are here for convenience
		// as this type is the universal color type it
		// makes it easier to construct these items
		// the same could be implemented as conversion operators
		tinyvg_rgba8888_t() :r(0), g(0), b(0), a(0) {}
		tinyvg_rgba8888_t(int r, int g, int b, int a=255)
			:r(r),g(g),b(b),a(a)
		{
		}
	};

	// Each color encoded as a sequence of two bytes
	union tinyvg_rgb565_t {
		uint16_t v;
		struct {
			uint16_t r : 5;
			uint16_t g : 6;
			uint16_t b : 5;
		};
	};

	// each color channel between 0.0 and 1.0
	struct tinyvg_rgbaf32_t {
		float r;
		float g;
		float b;
		float a;
	};
	
	struct tvg_point {
		float x;
		float y;
	};

	struct tvg_style_t
	{
		int kind;	// flat, linear, radial
		int id;		// and id that can be used on linear, or radial
		tvg_point point_0;
		tvg_point point_1;

		int color_index_0;
		int color_index_1;

		tinyvg_rgba8888_t color_0;
		tinyvg_rgba8888_t color_1;
	};

	// Representation of a single element of a countour
	// these are the path elements
	// using anonymous type union makes it easy to capture
	// contour commands
	struct tvg_contour_command_t {
		int kind;			// what kind of path command is it
		bool hasLineWidth;	// is the lineWidth valid
		float lineWidth;	// what is the line width if it's specialized

		union {
			struct {
				tvg_point point_0;
			} moveTo;
			struct {
				float x;
				float y;
				float width;
				float height;
			} rect;
			struct {
				tvg_point point_0;
				tvg_point point_1;
			} line;
			struct {
				tvg_point point_1;
			} lineTo ;
			struct {
				float x;
			} hlineTo;
			struct vline {
				float y;
			} vlineTo;
			struct  {
				tvg_point control_0;
				tvg_point control_1;
				tvg_point point_1;
			}cubicBezierTo;
			struct  {
				tvg_point control_0;
				tvg_point point_1;
			}quadraticBezierTo;
			struct  {
				bool sweep;
				bool largeArc;
				float radius;
				tvg_point target;
			}arcCircleTo;
			struct  {
				int sweep;
				int largeArc;
				tvg_point radiusxy;
				float rotation;
				tvg_point target;
			}arcEllipseTo;
		};

		// Convenience constructor, to ensure the structure
		// starts out nice and clean
		tvg_contour_command_t(const int aKind)
		{
			memset(this, 0, sizeof(tvg_contour_command_t));
			kind = aKind;
		}
	};

	// A representation of a top level Tiny VR Command
	// This contains the command, style, lineWidth, and contour commands
	// This is the thing the parser fills in during the next() function call
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

	// simple structure to contain parsed header information
	struct tvg_header_t {
		char magic[2];
		uint8_t version;

		uint8_t fracscale;				// Used for creating Unit fractions
		uint8_t colorEncoding;		// How are colors represented in color table
		uint8_t coordinateRange;	// how big can coordinates be

		uint32_t width;			// Width of image
		uint32_t height;		// Height of image
		uint32_t colorCount;	// Number of entries in color table
	};


	// Primary structure of the parser itself
	struct tvgscanner
	{
		tvg_header_t header;
		std::vector<tinyvg_rgba8888_t> colorTable;
		BinReader& bs;
		bool isValid=false;
		bool endOfDocument = false;
		int gradientCount = 0;

		tvgscanner(BinReader& bs)
			:bs(bs)
		{
			init();
		}

		// We have an init so that we can reset the parser to a beginning
		// state and start using it again.
		// we read the header and color table from here.
		// WAA - could give a parameter to set a position within 
		// the stream to reset to.
		bool init()
		{
			// set everything to an initial state
			endOfDocument = false;
			isValid= false;
			colorTable.clear();
			memset(&header, 0, sizeof(header));

			// read header
			auto success = readHeader();
			if (!success)
				return false;
			isValid = true;
			// read color table
			success = readColorTable();

			return success;
		}

		// Read the meta information of the file
		// This is the first thing the parser does
		bool readHeader()
		{
			// read magic
			bs.readBytes((uint8_t*)&header.magic, 2);
			// check header values to ensure we're reading the right thing

			// read version
			// check version to ensure we're reading the right thing
			header.version = bs.readOctet();

			// read scale : u4
			// read color encoding : u2
			// read coordinate range : u2
			uint8_t bits = bs.readOctet();
			header.fracscale = GETBITS8(bits, 0, 3);
			header.colorEncoding = GETBITS8(bits, 4, 5);
			header.coordinateRange = GETBITS8(bits, 6, 7);

			// read width
			// read height
			header.width = readRangeInt();
			header.height = readRangeInt();

			// how many colors are in the color tables
			header.colorCount = readUInt();

			return true;
		}

		// The color table is located right after the header
		// Turn colors into an array of BLRgba values
		bool readColorTable()
		{
			for (int i = 0; i < header.colorCount; i++) 
			{
				// read a color and convert to BLRgba32
				switch (header.colorEncoding) {
				case ColorEncoding::RGB565:
				{
					tinyvg_rgb565_t c;
					c.v = bs.readUInt16();
					colorTable.push_back(tinyvg_rgba8888_t(c.r, c.g, c.b));
				}
				break;

				case ColorEncoding::RGBA8888:
				{
					tinyvg_rgba8888_t c;
					c.v = bs.readUInt32();
					colorTable.push_back(c);
				}

				break;

				case ColorEncoding::RGBAF32:
				{
					tinyvg_rgbaf32_t c;
					c.r = bs.readFloat();
					c.g = bs.readFloat();
					c.b = bs.readFloat();
					c.a = bs.readFloat();
					colorTable.push_back(tinyvg_rgba8888_t((int)(c.r * 255), (int)(c.g * 255), (int)(c.b * 255), (int)(c.a * 255)));
				}
				break;

				default:
					break;
				}
			}

			return true;
		}


		// next()
		// The meat and potatoes of the parser.  This will return the 
		// recognized item in the file stream, whether beginning
		// or ending of commands or contours
		bool next(tvg_command_t& cmd)
		{
			if (bs.isEOF() || endOfDocument)
				return false;

			uint8_t cmdBits = bs.readOctet();
			cmd.command = GETBITS8(cmdBits, 0, 5);
			uint8_t primary_style_kind = GETBITS8(cmdBits, 6, 7);

			switch (cmd.command)
			{
			case Commands::EndOfDocument:
			{
				//printf("EndOfDocument\n");
				endOfDocument = true;
				return false;
			}
			break;

			case Commands::DrawLineLoop:
			{
				readLineHeader(cmd, primary_style_kind);

				cmd.contours.push_back(tvg_contour_command_t(GeoCommands::path));

				readLineStrip(cmd);

				cmd.contours.push_back(tvg_contour_command_t(ContourCommands::closePath));
			}
			break;

			case Commands::DrawLinePath:
			{
				readLineHeader(cmd, primary_style_kind);
				readPath(cmd);
			}
			break;

			case Commands::DrawLines:
			{
				readLineHeader(cmd, primary_style_kind);

				// read sets of lines
				for (int i = 0; i < cmd.segment_count; i++) {
					tvg_contour_command_t linecmd(GeoCommands::line);
					linecmd.line.point_0 = readPoint();
					linecmd.line.point_1 = readPoint();
					cmd.contours.push_back(linecmd);
				}
			}
			break;

			case Commands::DrawLineStrip:
			{
				readLineHeader(cmd, primary_style_kind);
				readLineStrip(cmd);
			}
			break;

			case Commands::FillPath:
			{
				readFillHeader(cmd, primary_style_kind);
				readPath(cmd);
			}
			break;

			case Commands::FillPolygon:
			{
				readFillHeader(cmd, primary_style_kind);

				cmd.contours.push_back(tvg_contour_command_t(GeoCommands::path));

				readLineStrip(cmd);
				cmd.contours.push_back(tvg_contour_command_t(ContourCommands::closePath));
			}
			break;

			case Commands::FillRectangles:
			{
				readFillHeader(cmd, primary_style_kind);

				// read rectangles
				for (int i = 0; i < cmd.segment_count; i++)
				{
					tvg_contour_command_t geocmd(GeoCommands::rect);
					geocmd.rect.x = readUnit();
					geocmd.rect.y = readUnit();
					geocmd.rect.width = readUnit();
					geocmd.rect.height = readUnit();

					cmd.contours.push_back(geocmd);
				}
			}
			break;

			case Commands::OutlineFillPath:
			{
				readOutlineFillHeader(cmd, primary_style_kind);
				readPath(cmd);
			}
			break;

			case Commands::OutlineFillPolygon:
			{
				readOutlineFillHeader(cmd, primary_style_kind);
				cmd.contours.push_back(tvg_contour_command_t(GeoCommands::path));
				readLineStrip(cmd);
				cmd.contours.push_back(tvg_contour_command_t(ContourCommands::closePath));
			}
			break;

			case Commands::OutlineFillRectangles:
			{
				readOutlineFillHeader(cmd, primary_style_kind);

				// read rectangles
				for (int i = 0; i < cmd.segment_count; i++)
				{
					tvg_contour_command_t geocmd(GeoCommands::rect);
					geocmd.rect.x = readUnit();
					geocmd.rect.y = readUnit();
					geocmd.rect.width = readUnit();
					geocmd.rect.height = readUnit();

					cmd.contours.push_back(geocmd);
				}
			}
			break;

			default: {
				// Unknown command, simply return false
				printf("Unknown Command: %d\n", cmd.command);

				return false;
			}
				   break;
			}

			return true;
		}

		// Parsing routines
		// Read a variable sized integer from the stream
		// limited to unsigned 32-bit maximum
		// calculatring binary to decimal:
		// https://www.rapidtables.com/convert/number/binary-to-decimal.html?x=0001.0011
		//
		uint32_t readUInt()
		{
			uint32_t result = 0;

			for (int count = 0; count < 5; count++)	// at most 5 iterations
			{
				auto abyte = bs.readOctet();
				auto val = (abyte & 0x7f) << (7 * count);
				result |= val;

				if ((abyte & 0x80) == 0)
					break;
			}

			return result;
		}

		// A unit is encoded as a signed integer, with a configurable
		// amount of bits (CoordinateRange)
		uint32_t readRangeInt()
		{
			uint32_t value = 0;

			switch (header.coordinateRange)
			{
			case CoordinateRange::Default:
				// 16-bit
				value = bs.readUInt16();

				break;
			case CoordinateRange::Reduced:
				// 8-bit
				value = bs.readOctet();
				break;
			case CoordinateRange::Enhanced:
				// 32-bit
				value = bs.readUInt32();
				break;
			}

			return value;
		}

		// Read a fixed value and turn it into floating point
		float readUnit()
		{
			uint32_t value = readRangeInt();
			return (float)value / (float)(1 << header.fracscale);
		}

		// Read an x,y pair of Units
		tvg_point readPoint()
		{
			tvg_point pt;
			pt.x = readUnit();
			pt.y = readUnit();

			return pt;
		}

		// Read a single style attribute
		tvg_style_t  readStyle(int kind)
		{
			tvg_style_t s;
			s.kind = kind;

			switch (s.kind) {
			case DrawingStyle::FlatColored:
			{
				s.color_index_0 = readUInt();
				s.color_0 = colorTable[s.color_index_0];
			}
				break;

			case DrawingStyle::LinearGradient: 
			case DrawingStyle::RadialGradient: 
			{
				gradientCount++;
				s.id = gradientCount;
				s.point_0 = readPoint();
				s.point_1 = readPoint();
				s.color_index_0 = readUInt();
				s.color_index_1 = readUInt();

				s.color_0 = colorTable[s.color_index_0];
				s.color_1 = colorTable[s.color_index_1];
			}
			break;

			}
			return s;
		}


		// Read the header associated with a line command
		// DrawLineLoop
		// DrawLineStrip
		// DrawLinePath
		// DrawLines
		//
		bool readLineHeader(tvg_command_t& cmd, int primary_style_kind)
		{
			cmd.segment_count = readUInt() + 1;
			cmd.lineStyle = readStyle(primary_style_kind);
			cmd.lineWidth = readUnit();

			return true;
		}

		// Read header associated with fill commands
		// FillPath
		// FillPolygon
		// FillRectangles
		//
		bool readFillHeader(tvg_command_t& cmd, int primary_style_kind)
		{
			cmd.segment_count = readUInt()+1;
			cmd.fillStyle = readStyle(primary_style_kind);

			return true;
		}

		// Read header associated with fill and outline commands
		// OutlineFillPath
		// OutlineFillPolygon
		// OutlineFillRectangles
		bool readOutlineFillHeader(tvg_command_t& cmd, int primary_style_kind)
		{
			// segment count and style
			uint32_t bits = bs.readOctet();
			cmd.segment_count = GETBITS8(bits, 0, 5) + 1;
			int scondary_style_kind = GETBITS8(bits, 6, 7);


			cmd.fillStyle = readStyle(primary_style_kind);
			cmd.lineStyle = readStyle(scondary_style_kind);

			// line_width unit
			cmd.lineWidth = readUnit();

			return true;
		}

		bool readLineStrip(tvg_command_t &cmd)
		{
			tvg_contour_command_t movcmd(ContourCommands::moveTo);
			movcmd.moveTo.point_0 = readPoint();
			cmd.contours.push_back(movcmd);

			for (int i = 1; i < cmd.segment_count; i++)
			{
				tvg_contour_command_t linecmd(ContourCommands::lineTo);
				linecmd.lineTo.point_1 = readPoint();
				cmd.contours.push_back(linecmd);
			}
			return true;
		}

		// Read the components of a path used by
		//   DrawLinePath
		//   FillPath
		//   OutlineFillPath
		//
		// read as many segments as are indicated in the cmd.segment_count
		// fill them into the contours collection of the cmd
		//
		bool readPath(tvg_command_t &cmd)
		{
			// Get the segment lengths
			int total_node_count = 0;
			std::vector<int> segment_lengths;

			int i = 0;

			while (i < cmd.segment_count)
			{
				segment_lengths.push_back(readUInt() + 1) ;
				total_node_count += segment_lengths[i];

				i += 1;
			}

			// Stick a path geometry symbol at the beginning
			// of the contours collection
			cmd.contours.push_back(tvg_contour_command_t(GeoCommands::path));

			// tvg_contour_command_t
			// Each segment is comprised of a number of contours
			for (int sctr = 0; sctr < cmd.segment_count; sctr++)
			{
				int command_count = segment_lengths[sctr];

				// each contour begins with a moveTo
				// read start point here as it does not change
				tvg_contour_command_t movcmd(ContourCommands::moveTo);
				movcmd.hasLineWidth = 0;
				movcmd.moveTo.point_0 = readPoint();
				cmd.contours.push_back(movcmd);

				// For each contour
				for (int insCtr = 1; insCtr <= command_count; insCtr++)
				{
					// decode attributes from first byte
					uint8_t insBits = bs.readOctet();
					auto kind = GETBITS8(insBits, 0, 2);
					auto hasLineWidth = GETBITS8(insBits, 4, 4);
					uint8_t padding = GETBITS8(insBits, 3, 3);	// always 0
					uint8_t morepadding = GETBITS8(insBits, 5, 7);	// always 0

					tvg_contour_command_t contourCmd(kind);
					contourCmd.hasLineWidth = hasLineWidth;

					if (contourCmd.hasLineWidth)
						contourCmd.lineWidth = readUnit();

					// now read the specifics of the commands
					readContourCommand(contourCmd);

					cmd.contours.push_back(contourCmd);
				}
			}

			return true;
		}

		//
		// A path segment is a piece of geometry with a maximum of 64 points.
		// other than the line width, there is no other drawing info
		// here.
		// Need to capture the line width if it's specified and
		// convey it back.
		bool readContourCommand(tvg_contour_command_t &cmd)
		{
			switch (cmd.kind)
			{
				case ContourCommands::lineTo :
				{
					cmd.lineTo.point_1 = readPoint();
				}
				break;
				case ContourCommands::hlineTo :
				{
					cmd.hlineTo.x = readUnit();
				}
				break;
				case ContourCommands::vlineTo :
				{
					cmd.vlineTo.y = readUnit();
				}
				break;

				case ContourCommands::cubicBezierTo :
				{
					cmd.cubicBezierTo.control_0 = readPoint();
					cmd.cubicBezierTo.control_1 = readPoint();
					cmd.cubicBezierTo.point_1 = readPoint();
				}
				break;

				case ContourCommands::quadraticBezierTo :
				{
					cmd.quadraticBezierTo.control_0 = readPoint();
					cmd.quadraticBezierTo.point_1 = readPoint();
				}
				break;

				case ContourCommands::arcCircleTo :
				{
					uint8_t flags = bs.readOctet();
					cmd.arcCircleTo.largeArc = GETBITS8(flags, 0, 0);
					cmd.arcCircleTo.sweep = !GETBITS8(flags, 1, 1);
					//int padding = GETBITS8(flags, 2, 7);	// always 0

					cmd.arcCircleTo.radius = readUnit();
					cmd.arcCircleTo.target = readPoint();
				}
				break;

				case ContourCommands::arcEllipseTo :
				{
					uint8_t flags = bs.readOctet();
					cmd.arcEllipseTo.sweep = !GETBITS8(flags, 1, 1);	// 0 == anti-clockwise, 1 == clockwise
					cmd.arcEllipseTo.largeArc = ((flags & 1) != 0);
					//int padding = GETBITS8(flags, 2, 7);

					cmd.arcEllipseTo.radiusxy = readPoint();
					cmd.arcEllipseTo.rotation = readUnit();
					cmd.arcEllipseTo.target = readPoint();
				}
				break;

				case ContourCommands::closePath :
					break;
			}

			return true;
		}
	};

}
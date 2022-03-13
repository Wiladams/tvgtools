#pragma once

#include <functional>
#include "tvgscan.h"

using namespace tvg;



class TVGScanEncoder
{
public:
	TVGScanEncoder(void){}
	virtual ~TVGScanEncoder(void){}

	// Draw the entire thing in one shot
	virtual void writeDocument(tvgscanner& p)	
	{
		writeDocumentHeader(p.header);
		writeTokens(p);
		writeDocumentFooter();
	}

	virtual void writeDocumentHeader(tvg_header_t &header)
	{
		printf("DOCUMENT FOOTER\n");
	}

	virtual void writeDocumentFooter()
	{
		printf("DOCUMENT FOOTER\n");
	}



    virtual void writeStyle(const char *name, tvg_style_t& tvgs)
	{
	switch (tvgs.kind)
	{
	case DrawingStyle::FlatColored:
	{
		printf("ctx.set%sStyle(",name);
		printf("BLRgba32(%d,%d,%d,%d));\n",
			tvgs.color_0.r, tvgs.color_0.g, tvgs.color_0.b, tvgs.color_0.a);
	}
	break;

	case DrawingStyle::LinearGradient:
	{
		printf("  BLGradient %slinear(BLLinearGradientValues(%3.3f,%3.3f,%3.3f,%3.3f));\n", name, tvgs.point_0.x, tvgs.point_0.y, tvgs.point_1.x, tvgs.point_1.y);
		printf("  %slinear.addStop(0, BLRgba32(%d,%d,%d,%d));\n", name, tvgs.color_0.r, tvgs.color_0.g, tvgs.color_0.b, tvgs.color_0.a);
		printf("  %slinear.addStop(1, BLRgba32(%d,%d,%d,%d));\n", name, tvgs.color_1.r, tvgs.color_1.g, tvgs.color_1.b, tvgs.color_1.a);
		printf("  ctx.set%sStyle(%slinear);\n", name, name);
	}
	break;

	case DrawingStyle::RadialGradient:
	{
		double dx = tvgs.point_1.x - tvgs.point_0.x;
		double dy = tvgs.point_1.y - tvgs.point_0.y;
		double r0 = ::sqrt((dx * dx) + (dy * dy));	// distance

		printf("  BLGradient %sradial(BLRadialGradientValues(%3.3f,%3.3f,%3.3f,%3.3f,%3.3f));\n", name, tvgs.point_0.x, tvgs.point_0.y, tvgs.point_0.x, tvgs.point_0.y, r0);
		printf("  %sradial.addStop(0, BLRgba32(%d,%d,%d,%d));\n", name, tvgs.color_0.r, tvgs.color_0.g, tvgs.color_0.b, tvgs.color_0.a);
		printf("  %sradial.addStop(1, BLRgba32(%d,%d,%d,%d));\n", name, tvgs.color_1.r, tvgs.color_1.g, tvgs.color_1.b, tvgs.color_1.a);
		printf("  ctx.set%sStyle(%sradial);\n", name, name);
	}
	break;

	}

}



	virtual void writePathEnd()
	{
		printf("\nPATH END\n");
	}

// Construct a path from contour commands
virtual void writeContour(tvg_command_t geo, tvg_contour_command_t& cmd )
{
	switch (cmd.kind)
	{
	case ContourCommands::moveTo:
		printf("segment.moveTo(%3.3f, %3.3f);\n", cmd.moveTo.point_0.x, cmd.moveTo.point_0.y);
	break;

	case ContourCommands::lineTo:
	{
		printf("segment.lineTo(%3.3f,%3.3f);\n", cmd.lineTo.point_1.x, cmd.lineTo.point_1.y);
	}
	break;
	
	case ContourCommands::hlineTo:
	{
		printf("segment.getLastVertex(&vtxOut);\n");
		printf("segment.lineTo(%3.3f, vtxOut.y);\n", cmd.hlineTo.x);
	}
	break;
	case ContourCommands::vlineTo:
	{
		printf("segment.getLastVertex(&vtxOut);\n");
		printf("segment.lineTo(vtxOut.x, %3.3f);\n", cmd.vlineTo.y);
	}
	break;
	
	case ContourCommands::cubicBezierTo:
	{
		printf("segment.cubicTo(%3.3f,%3.3f,%3.3f,%3.3f,%3.3f,%3.3f);\n",
			cmd.cubicBezierTo.control_0.x, cmd.cubicBezierTo.control_0.y,
			cmd.cubicBezierTo.control_1.x, cmd.cubicBezierTo.control_1.y,
			cmd.cubicBezierTo.point_1.x, cmd.cubicBezierTo.point_1.y);
	}
	break;

	case ContourCommands::quadraticBezierTo:
	{
		printf("  segment.quadTo(%3.3f,%3.3f,%3.3f,%3.3f);\n",
			cmd.quadraticBezierTo.control_0.x, cmd.quadraticBezierTo.control_0.y,
			cmd.quadraticBezierTo.point_1.x, cmd.quadraticBezierTo.point_1.y);
	}
	break;
	
	case ContourCommands::arcCircleTo:
	{
		printf("  segment.ellipticArcTo(BLPoint(%3.3f, %3.3f), 0, %d, %d, BLPoint(%3.3f,%3.3f));\n",
			cmd.arcCircleTo.radius, cmd.arcCircleTo.radius,
			cmd.arcCircleTo.largeArc, cmd.arcCircleTo.sweep,
			cmd.arcCircleTo.target.x, cmd.arcCircleTo.target.y);
	}
	break;

	case ContourCommands::arcEllipseTo:
	{
		printf("  segment.ellipticArcTo(BLPoint(%3.3f, %3.3f), %3.3f, %d, %d, BLPoint(%3.3f,%3.3f));\n",
			cmd.arcEllipseTo.radiusxy.x, cmd.arcEllipseTo.radiusxy.y,
			cmd.arcEllipseTo.rotation,
			cmd.arcEllipseTo.largeArc, cmd.arcEllipseTo.sweep,
			cmd.arcEllipseTo.target.x, cmd.arcEllipseTo.target.y);
	}
	break;

	case ContourCommands::closePath:
		printf("segment.close();\n");
		break;

	}
}

virtual void writeGeoLine(tvg_command_t &geo, tvg_contour_command_t & cmd)
{
	printf("GEO LINE\n");
}

virtual void writeGeoPath(tvg_command_t &geo, tvg_contour_command_t & cmd)
{
	printf("GEO PATH\n");
}

virtual void writeGeoRect(tvg_command_t &geo, tvg_contour_command_t & cmd)
{
	printf("GEO RECT\n");
}


void writeContours(tvg_command_t &geo)
{
	for (auto& cmd : geo.contours)
	{
		switch(cmd.kind)
		{
			case GeoCommands::rect:
				writeGeoRect(geo, cmd);
			break;

			case GeoCommands::line:
				writeGeoLine(geo, cmd);
			break;

			case GeoCommands::path:
				writeGeoPath(geo, cmd);
			break;


			// assume it's the contours of a path
			default:
				writeContour(geo, cmd);
			break;
		}
	}
}

virtual void writeFillPath(tvg_style_t& s)
{
	writeStyle("Fill", s);
	printf("    ctx.fillPath(segment);\n");
}

virtual void writeStrokePath(tvg_style_t& s, float lineWidth)
{
	printf("  ctx.setStrokeWidth(%3.3f);\n", lineWidth);
	writeStyle("Stroke", s);
	printf("  ctx.strokePath(segment);\n");
}

virtual void writeCommand(tvg_command_t& cmd)
{
	writeContours(cmd);

	switch (cmd.command) {
		case Commands::DrawLinePath:
		case Commands::DrawLines:
		case Commands::DrawLineLoop:
		case Commands::DrawLineStrip:
		{
			writeStrokePath(cmd.lineStyle, cmd.lineWidth);
		}
		break;

		case Commands::FillPath:
		case Commands::FillPolygon:
		case Commands::FillRectangles:
		{
			writeFillPath(cmd.fillStyle);
		}
		break;

		case Commands::OutlineFillPath:
		case Commands::OutlineFillRectangles:
		case Commands::OutlineFillPolygon:
		{
			writeFillPath(cmd.fillStyle);
			writeStrokePath(cmd.lineStyle, cmd.lineWidth);
		}
		break;
	}
	
	writePathEnd();
}


virtual void writeTokens(tvgscanner &scanner)
{
	while (true)
	{
		tvg_command_t cmd;
		if (!scanner.next(cmd))
			break;

		writeCommand(cmd);
	}
}


};
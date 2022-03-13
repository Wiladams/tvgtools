#pragma once

#include "tvgscanencoder.h"

using namespace tvg;

class SVGEncoder : public TVGScanEncoder
{
    int gradientCount=0;

public:
	SVGEncoder(void){};
	~SVGEncoder(void){};

	void writeDocumentHeader(tvg_header_t &header)
	{
		printf("<svg width ='%d' height = '%d'  version = '1.1' xmlns='http://www.w3.org/2000/svg'>\n", header.width, header.height);
	}

	void writeDocumentFooter()
	{
		printf("\n</svg>\n");
	}

    void writeStyle(const char *name, tvg_style_t& tvgs)
    {
	    switch (tvgs.kind)
	    {
	        case DrawingStyle::FlatColored:
	        {
		        printf(" %s = 'rgb(%d,%d,%d,%d)' ",name,tvgs.color_0.r, tvgs.color_0.g, tvgs.color_0.b, tvgs.color_0.a);
            }
	        break;

	        case DrawingStyle::LinearGradient:
	        {
                gradientCount++;
                printf("\n<defs>\n");
                printf("\n <linearGradient id='gradientID%d' x1='%3.3f', y1 = '%3.3f' x2='%3.3f' y2='%3.3f'>\n", tvgs.id, tvgs.point_0.x, tvgs.point_0.y, tvgs.point_1.x, tvgs.point_1.y);
		        printf("   <stop offset = '0' stop-color = 'rgb(%d,%d,%d,%d)' />\n", tvgs.color_0.r, tvgs.color_0.g, tvgs.color_0.b, tvgs.color_0.a);
		        printf("   <stop offset = '1' stop-color = 'rgb(%d,%d,%d,%d)' />\n", tvgs.color_1.r, tvgs.color_1.g, tvgs.color_1.b, tvgs.color_1.a);
		        printf(" </linearGradient>\n");
                printf("</defs>\n");
            }
	        
            break;

	        case DrawingStyle::RadialGradient:
	        {
                gradientCount++;
		        double dx = tvgs.point_1.x - tvgs.point_0.x;
		        double dy = tvgs.point_1.y - tvgs.point_0.y;
		        double r0 = ::sqrt((dx * dx) + (dy * dy));	// distance
                printf("\n<defs>\n");
		        printf("\n<radialGradient id= ' gradientID%d' cx='%3.3f' cy='%3.3f' r='%3.3f'>\n", tvgs.id, tvgs.point_0.x, tvgs.point_0.y, r0);
		        printf("   <stop offset = '0' stop-color = 'rgb(%d,%d,%d,%d)' />\n", tvgs.color_0.r, tvgs.color_0.g, tvgs.color_0.b, tvgs.color_0.a);
		        printf("   <stop offset = '1' stop-color = 'rgb(%d,%d,%d,%d)' />\n", tvgs.color_1.r, tvgs.color_1.g, tvgs.color_1.b, tvgs.color_1.a);
		        printf("  </radialGradient>\n");
                                printf("</defs>\n");
            }
	        break;
	    }

    }


virtual void writeGeoLine(tvg_command_t &geo, tvg_contour_command_t & cmd)
{
	printf("\n<line x1='%3.3f' y1='%3.3f' x2='%3.3f' y2='%3.3f' />\n",
		cmd.line.point_0.x,
		cmd.line.point_0.y,
		cmd.line.point_1.x,
		cmd.line.point_1.y);
}

virtual void writeGeoPath(tvg_command_t &geo, tvg_contour_command_t & cmd)
{
	printf("<path d=\"");
}

virtual void writeGeoRect(tvg_command_t &geo, tvg_contour_command_t & cmd)
{
	printf("\n<rect x='%3.3f' y='%3.3f' width='%3.3f' height='%3.3f' />\n", 
		cmd.rect.x, cmd.rect.y, cmd.rect.width, cmd.rect.height);
}

virtual void writePathmoveTo(tvg_command_t &geo, tvg_contour_command_t & cmd)
{
	printf("M %3.3f,%3.3f ", cmd.moveTo.point_0.x, cmd.moveTo.point_0.y);
}

virtual void writePathlineTo(tvg_command_t &geo, tvg_contour_command_t & cmd)
{
	printf(" L %3.3f %3.3f ", cmd.lineTo.point_1.x, cmd.lineTo.point_1.y);
}

virtual void writePathhlineTo(tvg_command_t &geo, tvg_contour_command_t & cmd)
{
	printf(" H %3.3f", cmd.hlineTo.x);
}

virtual void writePathvlineTo(tvg_command_t &geo, tvg_contour_command_t & cmd)
{
	printf(" V %3.3f", cmd.vlineTo.y);
}

virtual void writePathcubicBezierTo(tvg_command_t &geo, tvg_contour_command_t & cmd)
{
	printf(" C %3.3f %3.3f %3.3f %3.3f %3.3f %3.3f ",
		cmd.cubicBezierTo.control_0.x, cmd.cubicBezierTo.control_0.y,
		cmd.cubicBezierTo.control_1.x, cmd.cubicBezierTo.control_1.y,
		cmd.cubicBezierTo.point_1.x, cmd.cubicBezierTo.point_1.y);
}

virtual void writePathquadraticBezierTo(tvg_command_t &geo, tvg_contour_command_t & cmd)
{
	printf(" Q %3.3f %3.3f %3.3f %3.3f ",
		cmd.quadraticBezierTo.control_0.x, cmd.quadraticBezierTo.control_0.y,
		cmd.quadraticBezierTo.point_1.x, cmd.quadraticBezierTo.point_1.y);
}

virtual void writePathclosePath(tvg_command_t &geo, tvg_contour_command_t & cmd)
{
	printf(" Z");
}

virtual void writePatharcCircleTo(tvg_command_t &geo, tvg_contour_command_t & cmd)
{
	printf("  A %3.3f %3.3f 0 %d %d %3.3f %3.3f ",
			cmd.arcCircleTo.radius, cmd.arcCircleTo.radius,
			cmd.arcCircleTo.largeArc, !cmd.arcCircleTo.sweep,
			cmd.arcCircleTo.target.x, cmd.arcCircleTo.target.y);
}
virtual void writePatharcEllipseTo(tvg_command_t &geo, tvg_contour_command_t & cmd)
{
	printf(" A %3.3f %3.3f %3.3f %d %d %3.3f %3.3f ",
		cmd.arcEllipseTo.radiusxy.x, cmd.arcEllipseTo.radiusxy.y,
		cmd.arcEllipseTo.rotation,
		cmd.arcEllipseTo.largeArc, !cmd.arcEllipseTo.sweep,
		cmd.arcEllipseTo.target.x, cmd.arcEllipseTo.target.y);
}

// Construct a path from contour commands
// This can be represented as a table of functions that
// have the command as the key.
void writeContour(tvg_command_t geo, tvg_contour_command_t& cmd)
{
	switch (cmd.kind)
	{
	case ContourCommands::moveTo:
		writePathmoveTo(geo, cmd);
	break;

	case ContourCommands::lineTo:
		writePathlineTo(geo, cmd);
	break;
	
	case ContourCommands::hlineTo:
		writePathhlineTo(geo,cmd);
	break;

	case ContourCommands::vlineTo:
		writePathvlineTo(geo, cmd);
	break;
	
	case ContourCommands::cubicBezierTo:
		writePathcubicBezierTo(geo, cmd);
	break;

	case ContourCommands::quadraticBezierTo:
		writePathquadraticBezierTo(geo, cmd);
	break;
	
	case ContourCommands::arcCircleTo:
		writePatharcCircleTo(geo, cmd);
	break;

	case ContourCommands::arcEllipseTo:
		writePatharcEllipseTo(geo, cmd);
	break;

	case ContourCommands::closePath:
		writePathclosePath(geo, cmd);
		break;
	}
}

void writePathEnd()
{
	printf("/>\n");
}

void writeStrokeWidth(float lineWidth)
{
    printf(" stroke-width=\"%3.3f\"", lineWidth);
}

void writeFillPath(tvg_style_t& s)
{
	writeStyle("fill", s);
}

void writeStrokePath(tvg_style_t& s, float lineWidth)
{
	writeStyle("stroke", s);
    writeStrokeWidth(lineWidth);
}

void writeCommand(tvg_command_t& cmd)
{
	switch (cmd.command) {
		case Commands::DrawLines:{
			writeContours(cmd);
        }break;

		case Commands::DrawLinePath:
		case Commands::DrawLineLoop:
		case Commands::DrawLineStrip:
		{
			writeContours(cmd);
    		printf("\"");	// closing path quote

			if (cmd.lineStyle.kind == DrawingStyle::FlatColored)
			{
				writeStrokePath(cmd.lineStyle, cmd.lineWidth);
				writePathEnd();
			}
			else
			{
				writePathEnd();
				writeStrokePath(cmd.lineStyle, cmd.lineWidth);
			}
		}
		break;

		case Commands::FillRectangles:{
            writeContours(cmd);
			// need to deal with style
        }break;

		case Commands::FillPath:
		case Commands::FillPolygon:
		{
	        writeContours(cmd);
            printf("\"");

			if (cmd.lineStyle.kind == DrawingStyle::FlatColored)
			{
				writeFillPath(cmd.fillStyle);
				writePathEnd();
			}
			else
			{
				writePathEnd();
				writeFillPath(cmd.fillStyle);
			}
		}
		break;

		case Commands::OutlineFillRectangles:{
            writeContours(cmd);
			// write style
        }break;

		case Commands::OutlineFillPath:
		case Commands::OutlineFillPolygon:
		{
	        writeContours(cmd);
            printf("\"");

			if (cmd.lineStyle.kind == DrawingStyle::FlatColored)
			{
				writeFillPath(cmd.fillStyle);
				writeStrokePath(cmd.lineStyle, cmd.lineWidth);
				writePathEnd();
			}
			else
			{
				writePathEnd();
				writeFillPath(cmd.fillStyle);
				writeStrokePath(cmd.lineStyle, cmd.lineWidth);
			}
		}
		break;
	}
}
};

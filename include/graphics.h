#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "types.h"
#include <vector>

// Line drawing algorithms
void drawLineDDA(float x1, float y1, float x2, float y2, Color color);
void drawLineBresenham(int x1, int y1, int x2, int y2, Color color, int thickness = 1);

// Circle drawing
void drawCircleMidpoint(int centerX, int centerY, int radius, Color color, bool filled = false);

void drawCircleBresenham(int xc, int yc, int r, Color color, bool filled= false);
// Fill algorithms
void scanLineFill(std::vector<Point>& vertices, Color fillColor, 
                  Color gradientColor = Color(), bool useGradient = false);

// Clipping
int computeOutCode(float x, float y, float xmin, float ymin, float xmax, float ymax);
bool cohenSutherlandClip(float& x1, float& y1, float& x2, float& y2,
                         float xmin, float ymin, float xmax, float ymax);

#endif
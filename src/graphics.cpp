#include "graphics.h"
#include "constants.h"
#include <GL/glut.h>
#include <cmath>
#include <algorithm>
#include <map>
#include <list>

void drawLineDDA(float x1, float y1, float x2, float y2, Color color) {
    glColor4f(color.r, color.g, color.b, color.a);
    
    float dx = x2 - x1;
    float dy = y2 - y1;
    float steps = std::max(abs(dx), abs(dy));
    
    if (steps == 0) return;
    
    float xIncrement = dx / steps;
    float yIncrement = dy / steps;
    
    float x = x1, y = y1;
    
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i <= steps; i++) {
        glVertex2f(x, y);
        x += xIncrement;
        y += yIncrement;
    }
    glEnd();
    glPointSize(1.0f);
}

void drawLineBresenham(int x1, int y1, int x2, int y2, Color color, int thickness) {
    glColor4f(color.r, color.g, color.b, color.a);
    glPointSize(thickness);

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int xi = (x2 > x1) ? 1 : -1;
    int yi = (y2 > y1) ? 1 : -1;

    glBegin(GL_POINTS);

    // Check which slope region we’re in
    if (dy <= dx) {
        // slope < 1
        int D = 2 * dy - dx;
        int y = y1;

        for (int x = x1; x != x2 + xi; x += xi) {
            glVertex2i(x, y);
            if (D > 0) {
                y += yi;
                D += 2 * (dy - dx);
            } else {
                D += 2 * dy;
            }
        }
    } else {
        // slope >= 1
        int D = 2 * dx - dy;
        int x = x1;

        for (int y = y1; y != y2 + yi; y += yi) {
            glVertex2i(x, y);
            if (D > 0) {
                x += xi;
                D += 2 * (dx - dy);
            } else {
                D += 2 * dx;
            }
        }
    }

    glEnd();
    glPointSize(1.0f);
}


void plotCirclePoints(int xc, int yc, int x, int y) {
    glVertex2i(xc + x, yc + y);
    glVertex2i(xc - x, yc + y);
    glVertex2i(xc + x, yc - y);
    glVertex2i(xc - x, yc - y);
    glVertex2i(xc + y, yc + x);
    glVertex2i(xc - y, yc + x);
    glVertex2i(xc + y, yc - x);
    glVertex2i(xc - y, yc - x);
}

void drawCircleMidpoint(int xc, int yc, int r, Color color, bool filled) {
    glColor4f(color.r, color.g, color.b, color.a);

    //  Filled circle
    if (filled) {
        for (int y = -r; y <= r; y++) {
            int x = (int)sqrt(r * r - y * y);
            glBegin(GL_LINES);
            glVertex2i(xc - x, yc + y);
            glVertex2i(xc + x, yc + y);
            glEnd();
        }
        return;
    }

    int x = 0;
    int y = r;
    int p = 1 - r;   // Initial decision parameter

    glPointSize(2.0f);
    glBegin(GL_POINTS);

    plotCirclePoints(xc, yc, x, y);

    while (x <= y) {
        x++;
        if (p < 0) {
            // Midpoint is inside the circle
            p = p + 2 * x + 3;
        } else {
            // Midpoint is outside or on the circle
            y--;
            p = p + 2 * (x - y) + 5;
        }
        plotCirclePoints(xc, yc, x, y);
    }

    glEnd();
    glPointSize(1.0f);
}


// Bresenham Circle Helper Function
void drawCirclePointsGL(int xc, int yc, int x, int y, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_POINTS);
    glVertex2f(xc + x, yc + y);
    glVertex2f(xc - x, yc + y);
    glVertex2f(xc + x, yc - y);
    glVertex2f(xc - x, yc - y);
    glVertex2f(xc + y, yc + x);
    glVertex2f(xc - y, yc + x);
    glVertex2f(xc + y, yc - x);
    glVertex2f(xc - y, yc - x);
    glEnd();
}


void drawCircleBresenham(int xc, int yc, int r, Color color, bool filled) {
    glColor4f(color.r, color.g, color.b, color.a);

    // Filled circle
    if (filled) {
        for (int y = -r; y <= r; y++) {
            int x = (int)sqrt(r * r - y * y);
            glBegin(GL_LINES);
            glVertex2i(xc - x, yc + y);
            glVertex2i(xc + x, yc + y);
            glEnd();
        }
        return;
    }

    int x = 0, y = r;
    int d = 3 - 2 * r;

    glPointSize(2.0f);

    while (y >= x) {
        drawCirclePointsGL(xc, yc, x, y, color.r, color.g, color.b);

        if (d < 0)
            d = d + 4 * x + 6;
        else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }

    glPointSize(1.0f);
}



struct Edge {
    float x;
    float dx_dy;
    int ymax;
    bool operator<(const Edge &e) const { return x < e.x; }
};

void scanLineFill(std::vector<Point>& vertices, Color fillColor, Color gradientColor, bool useGradient) {
    if (vertices.size() < 3) return;

    std::map<int, std::list<Edge>> edgeTable;
    int ymin = 100000, ymax = -100000;

    //  Build the edge table
    for (size_t i = 0; i < vertices.size(); ++i) {
        int x1 = round(vertices[i].x);
        int y1 = round(vertices[i].y);
        int x2 = round(vertices[(i + 1) % vertices.size()].x);
        int y2 = round(vertices[(i + 1) % vertices.size()].y);

        if (y1 == y2) continue; // skip horizontal edges

        ymin = std::min({ymin, y1, y2});
        ymax = std::max({ymax, y1, y2});

        Edge e;
        if (y1 < y2) {
            e.x = x1;
            e.ymax = y2;
            e.dx_dy = float(x2 - x1) / float(y2 - y1);
            edgeTable[y1].push_back(e);
        } else {
            e.x = x2;
            e.ymax = y1;
            e.dx_dy = float(x1 - x2) / float(y1 - y2);
            edgeTable[y2].push_back(e);
        }
    }

    // Initialize the Active Edge Table (AET)
    std::list<Edge> AET;

    // For each scanline
    for (int y = ymin; y < ymax; ++y) {
        // Add new edges from ET to AET
        if (edgeTable.count(y)) {
            AET.splice(AET.end(), edgeTable[y]);
        }

        // Remove edges for which y == ymax
        AET.remove_if([y](const Edge &e) { return e.ymax <= y; });

        // Sort AET by x
        AET.sort();

        // Optional: gradient color based on scanline
        if (useGradient) {
            float t = (float)(y - ymin) / (float)(ymax - ymin);
            glColor4f(
                fillColor.r + t * (gradientColor.r - fillColor.r),
                fillColor.g + t * (gradientColor.g - fillColor.g),
                fillColor.b + t * (gradientColor.b - fillColor.b),
                fillColor.a
            );
        } else {
            glColor4f(fillColor.r, fillColor.g, fillColor.b, fillColor.a);
        }

        // Draw pairs of intersections as filled lines
        auto it = AET.begin();
        while (it != AET.end()) {
            auto itNext = std::next(it);
            if (itNext == AET.end()) break;

            glBegin(GL_LINES);
            glVertex2f(it->x, y);
            glVertex2f(itNext->x, y);
            glEnd();

            std::advance(it, 2);
        }

        // Update x for all edges in AET
        for (auto &e : AET) e.x += e.dx_dy;
    }
}

// Compute region code for a point
int computeOutCode(float x, float y, float xmin, float ymin, float xmax, float ymax) {
    int code = INSIDE;

    if (x < xmin) code |= LEFT;
    else if (x > xmax) code |= RIGHT;

    if (y < ymin) code |= BOTTOM;
    else if (y > ymax) code |= TOP;

    return code;
}

// Cohen–Sutherland Line Clipping
bool cohenSutherlandClip(float& x1, float& y1, float& x2, float& y2,
                         float xmin, float ymin, float xmax, float ymax) {
    int outcode1 = computeOutCode(x1, y1, xmin, ymin, xmax, ymax);
    int outcode2 = computeOutCode(x2, y2, xmin, ymin, xmax, ymax);
    bool accept = false;

    while (true) {
        if (!(outcode1 | outcode2)) {
            // Both points inside
            accept = true;
            break;
        } else if (outcode1 & outcode2) {
            // Both points share an outside zone - reject
            break;
        } else {
            // Line needs clipping
            float x, y;
            int outcodeOut = outcode1 ? outcode1 : outcode2;

            if (outcodeOut & TOP) {
                x = x1 + (x2 - x1) * (ymax - y1) / (y2 - y1);
                y = ymax;
            } else if (outcodeOut & BOTTOM) {
                x = x1 + (x2 - x1) * (ymin - y1) / (y2 - y1);
                y = ymin;
            } else if (outcodeOut & RIGHT) {
                y = y1 + (y2 - y1) * (xmax - x1) / (x2 - x1);
                x = xmax;
            } else if (outcodeOut & LEFT) {
                y = y1 + (y2 - y1) * (xmin - x1) / (x2 - x1);
                x = xmin;
            }

            if (outcodeOut == outcode1) {
                x1 = x; y1 = y;
                outcode1 = computeOutCode(x1, y1, xmin, ymin, xmax, ymax);
            } else {
                x2 = x; y2 = y;
                outcode2 = computeOutCode(x2, y2, xmin, ymin, xmax, ymax);
            }
        }
    }

    return accept;
}
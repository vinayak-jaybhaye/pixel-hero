#include "graphics.h"
#include "constants.h"
#include <GL/glut.h>
#include <cmath>
#include <algorithm>

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
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    
    glBegin(GL_POINTS);
    while (true) {
        glVertex2i(x1, y1);
        
        if (x1 == x2 && y1 == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
    glEnd();
    glPointSize(1.0f);
}

void drawCircleMidpoint(int centerX, int centerY, int radius, Color color, bool filled) {
    glColor4f(color.r, color.g, color.b, color.a);
    
    if (filled) {
        for (int y = -radius; y <= radius; y++) {
            int x = sqrt(radius * radius - y * y);
            glBegin(GL_LINES);
            glVertex2i(centerX - x, centerY + y);
            glVertex2i(centerX + x, centerY + y);
            glEnd();
        }
        return;
    }
    
    int x = 0;
    int y = radius;
    int p = 1 - radius;
    
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    
    auto plotCirclePoints = [&](int cx, int cy, int x, int y) {
        glVertex2i(cx + x, cy + y);
        glVertex2i(cx - x, cy + y);
        glVertex2i(cx + x, cy - y);
        glVertex2i(cx - x, cy - y);
        glVertex2i(cx + y, cy + x);
        glVertex2i(cx - y, cy + x);
        glVertex2i(cx + y, cy - x);
        glVertex2i(cx - y, cy - x);
    };
    
    plotCirclePoints(centerX, centerY, x, y);
    
    while (x < y) {
        x++;
        if (p < 0) {
            p += 2 * x + 1;
        } else {
            y--;
            p += 2 * (x - y) + 1;
        }
        plotCirclePoints(centerX, centerY, x, y);
    }
    glEnd();
    glPointSize(1.0f);
}

void scanLineFill(std::vector<Point>& vertices, Color fillColor, Color gradientColor, bool useGradient) {
    if (vertices.size() < 3) return;
    
    float minY = vertices[0].y, maxY = vertices[0].y;
    for (const auto& vertex : vertices) {
        minY = std::min(minY, vertex.y);
        maxY = std::max(maxY, vertex.y);
    }
    
    for (int y = minY; y <= maxY; y++) {
        std::vector<float> intersections;
        
        for (size_t i = 0; i < vertices.size(); i++) {
            size_t j = (i + 1) % vertices.size();
            
            if ((vertices[i].y <= y && vertices[j].y > y) || 
                (vertices[j].y <= y && vertices[i].y > y)) {
                
                float x = vertices[i].x + (y - vertices[i].y) * 
                         (vertices[j].x - vertices[i].x) / (vertices[j].y - vertices[i].y);
                intersections.push_back(x);
            }
        }
        
        std::sort(intersections.begin(), intersections.end());
        
        for (size_t i = 0; i < intersections.size(); i += 2) {
            if (i + 1 < intersections.size()) {
                glBegin(GL_LINES);
                
                if (useGradient) {
                    float t = (y - minY) / (maxY - minY);
                    glColor4f(
                        fillColor.r + t * (gradientColor.r - fillColor.r),
                        fillColor.g + t * (gradientColor.g - fillColor.g),
                        fillColor.b + t * (gradientColor.b - fillColor.b),
                        fillColor.a
                    );
                } else {
                    glColor4f(fillColor.r, fillColor.g, fillColor.b, fillColor.a);
                }
                
                glVertex2f(intersections[i], y);
                glVertex2f(intersections[i + 1], y);
                glEnd();
            }
        }
    }
}

int computeOutCode(float x, float y, float xmin, float ymin, float xmax, float ymax) {
    int code = INSIDE;
    
    if (x < xmin) code |= LEFT;
    else if (x > xmax) code |= RIGHT;
    
    if (y < ymin) code |= BOTTOM;
    else if (y > ymax) code |= TOP;
    
    return code;
}

bool cohenSutherlandClip(float& x1, float& y1, float& x2, float& y2,
                         float xmin, float ymin, float xmax, float ymax) {
    int outcode1 = computeOutCode(x1, y1, xmin, ymin, xmax, ymax);
    int outcode2 = computeOutCode(x2, y2, xmin, ymin, xmax, ymax);
    bool accept = false;
    
    while (true) {
        if (!(outcode1 | outcode2)) {
            accept = true;
            break;
        } else if (outcode1 & outcode2) {
            break;
        } else {
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
                x1 = x;
                y1 = y;
                outcode1 = computeOutCode(x1, y1, xmin, ymin, xmax, ymax);
            } else {
                x2 = x;
                y2 = y;
                outcode2 = computeOutCode(x2, y2, xmin, ymin, xmax, ymax);
            }
        }
    }
    
    return accept;
}
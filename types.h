#ifndef TYPES_H
#define TYPES_H

#include <cmath>

// Color structure
struct Color {
    float r, g, b, a;
    Color(float red = 1.0f, float green = 1.0f, float blue = 1.0f, float alpha = 1.0f) 
        : r(red), g(green), b(blue), a(alpha) {}
};

// Point structure
struct Point {
    float x, y;
    Point(float px = 0, float py = 0) : x(px), y(py) {}
};

// 2D Transformation matrix
struct Transform2D {
    float matrix[3][3];
    
    Transform2D() {
        identity();
    }
    
    void identity() {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                matrix[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
    }
    
    void translate(float tx, float ty) {
        matrix[0][2] += tx;
        matrix[1][2] += ty;
    }
    
    void scale(float sx, float sy) {
        matrix[0][0] *= sx;
        matrix[1][1] *= sy;
    }
    
    void rotate(float angle) {
        float cosA = cos(angle);
        float sinA = sin(angle);
        float temp[3][3];
        
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                temp[i][j] = matrix[i][j];
            }
        }
        
        matrix[0][0] = temp[0][0] * cosA - temp[0][1] * sinA;
        matrix[0][1] = temp[0][0] * sinA + temp[0][1] * cosA;
        matrix[1][0] = temp[1][0] * cosA - temp[1][1] * sinA;
        matrix[1][1] = temp[1][0] * sinA + temp[1][1] * cosA;
    }
    
    Point transform(const Point& p) {
        float newX = matrix[0][0] * p.x + matrix[0][1] * p.y + matrix[0][2];
        float newY = matrix[1][0] * p.x + matrix[1][1] * p.y + matrix[1][2];
        return Point(newX, newY);
    }
};

#endif
#ifndef TYPES_H
#define TYPES_H

#include <cmath>

// -----------------------------
// Color structure
// -----------------------------
struct Color {
    float r, g, b, a;
    Color(float red = 1.0f, float green = 1.0f, float blue = 1.0f, float alpha = 1.0f)
        : r(red), g(green), b(blue), a(alpha) {}
};

// -----------------------------
// Point structure
// -----------------------------
struct Point {
    float x, y;
    Point(float px = 0, float py = 0) : x(px), y(py) {}
};

// -----------------------------
// 2D Transformation (Homogeneous 3x3 matrix)
// -----------------------------
struct Transform2D {
    float matrix[3][3];

    // Initialize as Identity Matrix
    Transform2D() { identity(); }

    void identity() {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                matrix[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
    }

    // Matrix multiplication
    void multiply(const float m2[3][3]) {
        float result[3][3] = {0};
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    result[i][j] += matrix[i][k] * m2[k][j];
                }
            }
        }
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                matrix[i][j] = result[i][j];
    }

    // Translation Transformation
    void translate(float tx, float ty) {
        float t[3][3] = {
            {1, 0, tx},
            {0, 1, ty},
            {0, 0, 1}
        };
        multiply(t);
    }

    // Scaling Transformation
    void scale(float sx, float sy) {
        float s[3][3] = {
            {sx, 0,  0},
            {0,  sy, 0},
            {0,  0,  1}
        };
        multiply(s);
    }

    // Rotation Transformation (angle in degrees)
    void rotate(float angle) {
        float rad = angle * M_PI / 180.0f;
        float c = cos(rad);
        float s = sin(rad);
        float r[3][3] = {
            {c, -s, 0},
            {s,  c, 0},
            {0,  0, 1}
        };
        multiply(r);
    }

    // Apply transformation to a point
    Point transform(const Point& p) {
        float newX = matrix[0][0] * p.x + matrix[0][1] * p.y + matrix[0][2];
        float newY = matrix[1][0] * p.x + matrix[1][1] * p.y + matrix[1][2];
        return Point(newX, newY);
    }
};

#endif

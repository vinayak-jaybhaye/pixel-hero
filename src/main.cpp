#include <GL/glut.h>
#include <iostream>
#include "game.h"
#include "constants.h"

// Global game instance
Game* game = nullptr;

// GLUT callback functions
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    if (game) {
        game->render();
    }
    
    glDisable(GL_BLEND);
    glutSwapBuffers();
}

void timer(int value) {
    if (game) {
        game->update();
    }
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // ~60 FPS
}

void update(int value) {
    if (game) {
        game->processInput();
    }
    glutTimerFunc(16, update, 0);
}

void keyboardDown(unsigned char key, int x, int y) {
    if (game) {
        game->handleKeyDown(key);
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    if (game) {
        game->handleKeyUp(key);
    }
}

void specialDown(int key, int x, int y) {
    if (game) {
        game->handleSpecialDown(key);
    }
}

void specialUp(int key, int x, int y) {
    if (game) {
        game->handleSpecialUp(key);
    }
}

void init() {
    glClearColor(0.6f, 0.8f, 1.0f, 1.0f);
    glPointSize(1.0f);
    glLineWidth(1.0f);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
}

void printInstructions() {
    std::cout << "==================================================================" << std::endl;
    std::cout << "   PIXEL HERO" << std::endl;
    std::cout << "==================================================================" << std::endl;
    std::cout << "CONTROLS:" << std::endl;
    std::cout << "• A/D or Arrow Keys - Move left/right" << std::endl;
    std::cout << "• W/Space/Up Arrow - Jump (Double Jump!)" << std::endl;
    std::cout << "• ESC - Exit game" << std::endl;
    std::cout << "==================================================================" << std::endl;
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_ALPHA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("PIXEL HERO");
    
    init();
    printInstructions();
    
    // Create game instance
    game = new Game();
    game->init();
    
    // Register GLUT callbacks
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialDown);
    glutSpecialUpFunc(specialUp);
    glutTimerFunc(0, timer, 0);
    glutTimerFunc(0, update, 0);
    
    glutMainLoop();
    
    // Cleanup
    delete game;
    return 0;
}
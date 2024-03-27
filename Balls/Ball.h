#pragma once
#include <GL/glut.h>
#include <iostream>
#include <numbers>

#if defined(__APPLE__)
const double PI = 3.14;
#else 
const double PI = std::numbers::pi;
#endif  
class Ball
{
public:
    Ball(float startX, float startY, float velocity, float startAngle);
    void draw();
    void move(float deltaTime);
    void invertDirection();
    void normalizeVelocity();
    void checkCanvasCollision();
    void drawScaled(float scale);

    // position
    float x;
    float y;

    float velocity;
    float angle;

    float dx;
    float dy;

    float radius = 5.0f;
};

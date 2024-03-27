#pragma once
#include <GL/glut.h>
#include <iostream>
#include <numbers>

class Sprite {
public:
    Sprite(); // Default constructor
    Sprite(float startX, float startY);
    void draw(float cameraX, float cameraY);

    void moveUp(float amount);
    void moveDown(float amount);
    void moveLeft(float amount);
    void moveRight(float amount);
    float getX() const; // Gets the X position
    float getY() const; // Gets the Y position

    void setX(float num);
    void setY(float num);

private:
    // position
    float x;
    float y;
    float height = 10.0f; 
    float radius = 5.0f;



};
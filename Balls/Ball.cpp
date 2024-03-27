#include "Ball.h"
#include <cmath>



Ball::Ball(float startX, float startY, float velocity, float startAngle)
    : x(startX), y(startY), velocity(velocity), angle(startAngle) {


        float angleRadians = startAngle * (PI / 180.0f);
        dx = velocity * std::cos(angleRadians);
        dy = velocity * std::sin(angleRadians);
    }


void Ball::draw() {
    glBegin(GL_TRIANGLE_FAN);
    glColor3ub(227, 151, 116);
    glVertex2f(x, y); // Center of circle
    for (int i = 0; i <= 20; i++) {
        float angle = 2.0f * PI * float(i) / float(20);  // Calculate the angle
        float xComponent = radius * cosf(angle); // Calculate the x component
        float yComponent = radius * sinf(angle); // Calculate the y component

        glVertex2f(x + xComponent, y + yComponent);
    }
    glEnd();
}

void Ball::move(float deltaTime) {
    // Predict the ball's next position based on its current velocity
    x += dx * deltaTime;
    y += dy * deltaTime;
}

void Ball::checkCanvasCollision() {
    float radiusThreshold = radius; 
    float leftBoundary = 0 + radiusThreshold;
    float rightBoundary = 1280 - radiusThreshold;
    float bottomBoundary = 0 + radiusThreshold;
    float topBoundary = 720 - radiusThreshold;

    if (x < leftBoundary || x > rightBoundary) {
        dx *= -1; 
        if (x < leftBoundary) {
            x = leftBoundary;
        }
        else {
            x = rightBoundary;
        }
    }

    if (y < bottomBoundary || y > topBoundary) {
        dy *= -1; 
        if (y < bottomBoundary) {
            y = bottomBoundary; 
        }
        else {
            y = topBoundary;
        }
    }
    normalizeVelocity();
}

void Ball::invertDirection() {
    dx *= -1;
    dy *= -1;
    normalizeVelocity();
}

void Ball::normalizeVelocity() {
    float velocityMagnitude = std::sqrt(dx * dx + dy * dy);
    dx = (dx / velocityMagnitude) * velocity;
    dy = (dy / velocityMagnitude) * velocity;
}

void Ball::drawScaled(float scaleFactor) {
    glBegin(GL_TRIANGLE_FAN);
    glColor3ub(227, 151, 116); // Set ball color
    for (int i = 0; i <= 20; i++) {
        float angle = 2.0f * PI * float(i) / float(20); // Calculate the angle
        float xComponent = (radius * scaleFactor) * cosf(angle); // Calculate the x component with scaling
        float yComponent = (radius * scaleFactor) * sinf(angle); // Calculate the y component with scaling

        glVertex2f(x + xComponent, y + yComponent);
    }
    glEnd();
}
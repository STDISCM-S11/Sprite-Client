#pragma once
#include <vector>
#include "Wall.h"
#include "Ball.h"

using namespace std; 

class BallManager{
public:
    static void addBall(const Ball& ball);
    static void addWall(const Wall& wall);
    static void updateBalls(float deltaTime);
    static void drawBalls(float cameraX, float cameraY, float peripheryWidth, float peripheryHeight, bool isExplorerMode, float zoomScale);
    static void drawWalls();
    static void addBallsDistance(int n, Point start, Point end, float velocity, float angle, int startSpawn, int endSpawn);
    static void addBallsAngle(int n, Point start, float velocity, float startAngle, float endAngle, int startSpawn, int endSpawn);
    static void addBallsVelocity(int n, Point position, float startVelocity, float endVelocity, float angle, int startSpawn, int endSpawn);
    static vector<Ball> getBalls();

private:
    static vector<Ball> balls;
    static vector<Wall> walls;
};


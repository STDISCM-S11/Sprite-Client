#include "BallManager.h"
#include "Wall.h"
#include "Ball.h"

#include <thread>
#include <mutex>

using namespace std;


vector<Ball> BallManager::balls;
vector<Wall> BallManager::walls;


void BallManager::addBall(const Ball& ball) {
    balls.push_back(ball);
}

void BallManager::addBallsDistance(int n, Point start, Point end, float velocity, float angle, int startSpawn, int endSpawn) {

    float distanceX = end.x - start.x;
    float distanceY = end.y - start.y;
    float totalDistance = sqrt(pow(distanceX, 2) + pow(distanceY, 2));
    float ballRadius = 5.0f;

    float spacing = totalDistance / (n - 1);

    float dirX = distanceX / totalDistance;
    float dirY = distanceY / totalDistance;

    for (int i = startSpawn; i < endSpawn; i++) {
        float posX = start.x + dirX * spacing * i;
        float posY = start.y + dirY * spacing * i;

        BallManager::addBall(Ball(posX, posY, velocity, angle));
    }
}

void BallManager::addBallsAngle(int n, Point position, float velocity, float startAngle, float endAngle, int startSpawn, int endSpawn) {

    float ballRadius = 5.0f;

    float angleIncrement = (n > 1) ? (endAngle - startAngle) / (n - 1) : 0;


    for (int i = startSpawn; i < endSpawn; i++) {
        float angleDegrees = startAngle + angleIncrement * i;

        BallManager::addBall(Ball(position.x, position.y, velocity, angleDegrees));
    }
}

void BallManager::addBallsVelocity(int n, Point position, float startVelocity, float endVelocity, float angle, int startSpawn, int endSpawn) {

    float velocityIncrement = (n > 1) ? (endVelocity - startVelocity) / (n - 1) : 0;

    for (int i = startSpawn; i < endSpawn; i++) {
        float currentVelocity = startVelocity + velocityIncrement * i;

        BallManager::addBall(Ball(position.x, position.y, currentVelocity, angle));
    }
}

void BallManager::updateBalls(float deltaTime) {

    const size_t numThreads = balls.size() > 1000 ? thread::hardware_concurrency(): 1;
    vector<thread> threads(numThreads);
    size_t ballsPerThread = balls.size() / numThreads;

    auto updateRange = [](int start, int end, float deltaTime) {
        for (int i = start; i < end; i++) {
            // Calculate the ball's next position based on its current velocity components
            bool collision = false;
            float nextX = balls[i].x + balls[i].dx * deltaTime;
            float nextY = balls[i].y + balls[i].dy * deltaTime;
            balls[i].checkCanvasCollision();

            // Move the ball after checking for collisions
            balls[i].move(deltaTime);
        }
    };

    for (int i = 0; i < numThreads; ++i) {
        int start = i * ballsPerThread;
        int end = (i + 1 == numThreads) ? balls.size() : (i + 1) * ballsPerThread;
        threads[i] = std::thread(updateRange, start, end, deltaTime);
    }

    for (auto& thread : threads) {
        thread.join(); // Wait for all threads to finish
    }
}

void BallManager::drawBalls() {
    for (auto& ball : balls) {
        ball.draw();
    }
}

void BallManager::addWall(const Wall& wall) {
    walls.push_back(wall);
}

void BallManager::drawWalls() {
    for (const auto& wall : walls) {
        wall.draw();
    }
}

vector<Ball> BallManager::getBalls() {
    return balls;
}
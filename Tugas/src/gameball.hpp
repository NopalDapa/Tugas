#ifndef GAMEBALL_HPP
#define GAMEBALL_HPP

#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;

class Ball {
public:
    Point position;      // Position of the ball
    Vec2f velocity;      // Velocity of the ball
    int radius;          // Radius of the ball
    bool visible;        // Visibility of the ball

    Ball(Point startPosition, Vec2f startVelocity, int r);
    void updatePosition();
    void reset(Point newPosition, Vec2f newVelocity);
};

class Game {
private:
    VideoCapture cap;   // Video capture object
    Ball ball;          // Ball object
    int score;          // Game score
    bool topWall, bottomWall, leftWall, rightWall; // Wall status

public:
    Game();
    void resetBall();
    void run();

private:
    void processFrame(Mat& frame);
    void checkCollision(const Mat& redMask, const Mat& frame);
    void handleWallCollisions(const Mat& frame);
};

#endif // GAMEBALL_HPP

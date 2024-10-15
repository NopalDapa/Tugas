#include "gameball.hpp"
#include <iostream>
#include <stdexcept>

Ball::Ball(Point startPosition, Vec2f startVelocity, int r)
    : position(startPosition), velocity(startVelocity), radius(r), visible(false) {}

void Ball::updatePosition() {
    position.x += static_cast<int>(velocity[0]);
    position.y += static_cast<int>(velocity[1]);
}

void Ball::reset(Point newPosition, Vec2f newVelocity) {
    position = newPosition;
    velocity = newVelocity;
    visible = true; // Make the ball visible again
}

Game::Game() 
    : cap(0), ball(Point(250, 250), Vec2f(5.0f, 3.0f), 20), score(0),
      topWall(true), bottomWall(true), leftWall(true), rightWall(true) {
    if (!cap.isOpened()) {
        throw std::runtime_error("Error opening video stream");
    }
    resetBall(); // Initialize the ball
}

void Game::resetBall() {
    int frameWidth = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH));
    int frameHeight = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT));
    int side = rand() % 4;

    int minSpeed = 5;
    int maxSpeed = 10;

    if (side == 0) { // Spawn from left
        ball.reset(Point(-ball.radius, rand() % frameHeight), Vec2f(rand() % (maxSpeed - minSpeed + 1) + minSpeed, (rand() % (maxSpeed - minSpeed + 1)) - (maxSpeed / 2)));
        leftWall = false;
    } else if (side == 1) { // Spawn from right
        ball.reset(Point(frameWidth + ball.radius, rand() % frameHeight), Vec2f(-(rand() % (maxSpeed - minSpeed + 1) + minSpeed), (rand() % (maxSpeed - minSpeed + 1)) - (maxSpeed / 2)));
        rightWall = false;
    } else if (side == 2) { // Spawn from top
        ball.reset(Point(rand() % frameWidth, -ball.radius), Vec2f((rand() % (maxSpeed - minSpeed + 1)) - (maxSpeed / 2), rand() % (maxSpeed - minSpeed + 1) + minSpeed));
        topWall = false;
    } else { // Spawn from bottom
        ball.reset(Point(rand() % frameWidth, frameHeight + ball.radius), Vec2f((rand() % (maxSpeed - minSpeed + 1)) - (maxSpeed / 2), -(rand() % (maxSpeed - minSpeed + 1) + minSpeed)));
        bottomWall = false;
    }
}

void Game::run() {
    while (true) {
        Mat frame;
        cap >> frame;

        if (frame.empty()) break;

        // Process the frame
        processFrame(frame);

        // Display the frame
        imshow("Bola", frame);
        if (waitKey(30) == 'q') break; // Wait for user input
    }

    cap.release();
    destroyAllWindows();
}

void Game::processFrame(Mat& frame) {
    // Convert to HSV
    Mat hsvFrame;
    cvtColor(frame, hsvFrame, COLOR_BGR2HSV);

    // Create masks for red color detection
    Scalar lowerRed1(0, 100, 100);
    Scalar upperRed1(10, 255, 255);
    Scalar lowerRed2(160, 100, 100);
    Scalar upperRed2(180, 255, 255);
    Mat mask1, mask2;
    inRange(hsvFrame, lowerRed1, upperRed1, mask1);
    inRange(hsvFrame, lowerRed2, upperRed2, mask2);
    Mat redMask = mask1 | mask2;

    // Find contours
    std::vector<std::vector<Point>> contours;
    findContours(redMask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // Draw bounding boxes around detected areas
    for (const auto& contour : contours) {
        Rect boundingBox = boundingRect(contour);
        rectangle(frame, boundingBox, Scalar(255, 255, 255), 2);
    }

    // Check for collision with red area
    checkCollision(redMask, frame);

    // Draw the ball if visible
    if (ball.visible) {
        circle(frame, ball.position, ball.radius, Scalar(0, 0, 255), -1);
        ball.updatePosition();
        handleWallCollisions(frame);
    }

    // Display score
    std::string scoreText = "Score: " + std::to_string(score);
    putText(frame, scoreText, Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
}

void Game::checkCollision(const Mat& redMask, const Mat& frame) {
    if (ball.visible) {
        Rect ballRect(ball.position.x - ball.radius, ball.position.y - ball.radius, ball.radius * 2, ball.radius * 2);
        ballRect &= Rect(0, 0, frame.cols, frame.rows);

        if (ballRect.width > 0 && ballRect.height > 0) {
            Mat ballRegion = redMask(ballRect);
            if (countNonZero(ballRegion) > 0) {
                ball.visible = false; // Ball disappears upon contact
                score++;
                resetBall(); // Spawn a new ball
            }
        }
    } else {
        // If the ball is not visible, reset its position
        resetBall();
    }
}

void Game::handleWallCollisions(const Mat& frame) {
    // Check for collisions with the walls
    if ((ball.position.x - ball.radius < 0 && leftWall) || (ball.position.x + ball.radius > frame.cols && rightWall)) {
        ball.velocity[0] = -ball.velocity[0]; // Reverse horizontal direction
    }
    if ((ball.position.y - ball.radius < 0 && topWall) || (ball.position.y + ball.radius > frame.rows && bottomWall)) {
        ball.velocity[1] = -ball.velocity[1]; // Reverse vertical direction
    }

    // Restore walls if the ball is within frame boundaries
    if (ball.position.x > ball.radius && ball.position.x < frame.cols - ball.radius) {
        leftWall = true;
        rightWall = true;
    }
    if (ball.position.y > ball.radius && ball.position.y < frame.rows - ball.radius) {
        topWall = true;
        bottomWall = true;
    }
}

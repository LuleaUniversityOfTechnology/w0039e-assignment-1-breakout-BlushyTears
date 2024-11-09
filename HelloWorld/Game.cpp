#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include "constants.h"
#include "Paddle.h"

// draws paddle
void DrawPaddle(Paddle paddle) {
	Play::DrawRect({ paddle.x, paddle.y }, { paddle.sizeX, paddle.sizeY }, Play::cWhite);
}

// Sets up paddle params, since they have to be initialized within a function
Paddle paddle;
void initializePaddle() {
	paddle.x = 100;
	paddle.y = 10;
	paddle.sizeX = 200;
	paddle.sizeY = 5;
}

// returns the largest of two integers
int Max(int x, int y) {
	return x > y ? x : y;
}

// returns the smallest of two integers
int Min(int x, int y) {
	return x < y ? x : y;
}

// Checks if the ball, as well as the global variable 'paddle' are colliding
bool isColliding(const GameObject& ball) {
	const float dx = ball.pos.x - Max(paddle.x, Min(ball.pos.x, paddle.sizeX));
	const float dy = ball.pos.y - Max(paddle.y, Min(ball.pos.y, paddle.sizeY));
	return (dx * dx + dy * dy) < (ball.radius * ball.radius);
}

// setup function that draws all the bricks in a nested loop
void spawnBricks() {
	for (int x = 0; x < 35; x++) {
		Play::CreateGameObject(ObjectType::TYPE_BRICK, { DISPLAY_WIDTH + x * 18, 0 }, 6, "brick");
		for (int y = 0; y < 5; y++) {
			Play::CreateGameObject(ObjectType::TYPE_BRICK, { 6 + x * 18, y * 12 + DISPLAY_HEIGHT - 65 }, 6, "brick");
		}
	}
}

// set up function that draws the ball or balls if multiple are set in constants.h
void spawnBall() {
	Play::CreateManager(DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE);
	for (int i = 0; i < NUM_OF_BALLS; i++) {
		const int objectId = Play::CreateGameObject(ObjectType::TYPE_BALL, { (DISPLAY_WIDTH + i + 10) / 2, DISPLAY_HEIGHT - 150 }, 4, "ball");
		GameObject& ball1 = Play::GetGameObject(objectId);
		ball1.velocity = normalize({ 5, -5 }) * BALLSPEED;
	}
}

// function that runs about 30 times a second i think
void StepFrame(float elapsedTime) {
	const std::vector<int> ballIds = Play::CollectGameObjectIDsByType(ObjectType::TYPE_BALL);
	const std::vector<int> brickIds = Play::CollectGameObjectIDsByType(ObjectType::TYPE_BRICK);
	std::vector<int> bricksToBeDestroyed;

	if (Play::KeyDown(KEY_RIGHT)) {
		paddle.x += 5;
		paddle.sizeX += 5;
	}
	if (Play::KeyDown(KEY_LEFT)) {
		paddle.x -= 5;
		paddle.sizeX -= 5;
	}

	// redraws paddle 
	DrawPaddle(paddle);

	for (int i = 0; i < ballIds.size(); i++) {
		GameObject& ball = Play::GetGameObject(ballIds[i]);
		Play::Point2D pos = ball.pos;

		if (pos.x > DISPLAY_WIDTH - 5 || pos.x < 0)
			ball.velocity.x *= -1;
		if (pos.y > DISPLAY_HEIGHT - 5 || pos.y < 0)
			ball.velocity.y *= -1;

		if (isColliding(ball)) {
			ball.velocity.y *= -1;
		}

		for (int j = 0; j < brickIds.size(); j++) {
			GameObject& brick = Play::GetGameObject(brickIds[j]);
			Play::Point2D pos = brick.pos;

			// saves a record of bricks to destroy later (destroying them here caused a lot of lag)
			if (Play::IsColliding(ball, brick)) {
				bricksToBeDestroyed.push_back(brick.GetId());
				ball.velocity.y *= -1;
			}
			Play::DrawObject(brick);
		}
		Play::DrawObject(ball);
		Play::UpdateGameObject(ball, false, 0, false);
	}

	// here we destroy the bricks based on the id's stored in the vector
	for (int brickId : bricksToBeDestroyed) {
		Play::DestroyGameObject(brickId);
	}
}

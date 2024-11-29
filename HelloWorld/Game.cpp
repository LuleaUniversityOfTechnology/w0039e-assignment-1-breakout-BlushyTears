#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include "constants.h"
#include "Paddle.h"
#include "MyList.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

MyList highscores(5);
unsigned int currentBrickScore = 0;

// function declarations
void spawnBall();
void drawText();

void createFile() {
	ofstream MyFile("filename.txt");

	for (int i = 0; i < highscores.getSize(); i++) {
		if (highscores[i] != 0)
			MyFile << highscores[i] << "\n";
	}

	MyFile.close();
}

void readFile() {
	if (std::filesystem::exists("filename.txt")) {

		ifstream MyReadFile("filename.txt");
		string line;

		int i = 0;
		while (getline(MyReadFile, line) && i < highscores.getSize()) {
			// Fail safe in case there is a newline created by a human, it will also concatenate if there are new lines in between scores
			// Which is just a nice to have
			if (line.empty()) {
				continue;
			}
			highscores[i] = stoi(line);
			i++;
		}
		MyReadFile.close();
	}
	else {
		for (int i = 0; i < 5; i++) {
			highscores[i] = 0;
		}
		createFile();
	}
}

// draws paddle
void DrawPaddle(Paddle const& paddle) {
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

void resetGame() {
	std::vector<int> brickIds = Play::CollectGameObjectIDsByType(ObjectType::TYPE_BRICK);
	std::vector<int> ballIds = Play::CollectGameObjectIDsByType(ObjectType::TYPE_BALL);

	for (int brick : brickIds) {
		Play::DestroyGameObject(brick);
	}
	for (int ball : ballIds) {
		Play::DestroyGameObject(ball);
	}
	spawnBricks();
	spawnBall();
}

// set up function that draws the ball or balls if multiple are set in constants.h
void spawnBall() {
	for (int i = 0; i < NUM_OF_BALLS; i++) {
		const int objectId = Play::CreateGameObject(ObjectType::TYPE_BALL, { (DISPLAY_WIDTH + i + 10) / 2, DISPLAY_HEIGHT - 150 }, 4, "ball");
		GameObject& ball1 = Play::GetGameObject(objectId);
		ball1.velocity = normalize({ 5, -5 }) * BALLSPEED;
	}
}

// function that runs about 30 times a second i think
void StepFrame(float elapsedTime) {
	std::vector<int> ballIds = Play::CollectGameObjectIDsByType(ObjectType::TYPE_BALL);
	std::vector<int> brickIds = Play::CollectGameObjectIDsByType(ObjectType::TYPE_BRICK);
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
		if (pos.y > DISPLAY_HEIGHT - 5)
			ball.velocity.y *= -1;

		if (pos.y < 0) {
			highscores.increaseSize(1);

			for (int i = 0; i < highscores.getSize(); i++) {
				if (currentBrickScore > highscores[i]) {
					MyList tempList(highscores.getSize() + 1);
					int index = 0;

					// These three for loops roughly emulate insert() from vector libary
					for (int j = 0; j < i; j++) {
						tempList[index++] = highscores[j];
					}

					tempList[index++] = currentBrickScore;


					for (int j = i; j < highscores.getSize(); j++) {
						tempList[index++] = highscores[j];
					}

					for (int j = 0; j < highscores.getSize(); j++) {
						highscores[j] = tempList[j];
					}


					currentBrickScore = 0;
					break;
				}
			}
			currentBrickScore = 0;
			resetGame();
			return;
		} 

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
				currentBrickScore++;
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

	drawText();
}

void drawText() {
	// Display the current player score
	std::string tempString = std::to_string(currentBrickScore);
	const char* myArray = tempString.c_str();
	Play::DrawDebugText({ 50, 50 }, myArray, Play::cBlue, true);

	for (int i = 5; i > 0; i--) {
		// first cast the int to a string, then turn it into a const char list which is what playbuffer expects
		std::string tempString = "P" + std::to_string(5 - i) + ": " + std::to_string(highscores[5 - i]);
		const char* myArray = tempString.c_str();
		Play::DrawDebugText({ DISPLAY_WIDTH - 30, (8 * (i - 1)) * 3 + 20 }, myArray, Play::cBlue, true);
	}
}


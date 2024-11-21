#include "Game.h"
#include "constants.h"

// The entry point for a PlayBuffer program
void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
	Play::CreateManager(DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE);
	readFile();
	spawnBall();
	spawnBricks();
	initializePaddle();
}

// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate( float elapsedTime )
{
	Play::ClearDrawingBuffer( Play::cBlack );
	StepFrame(elapsedTime);

	Play::PresentDrawingBuffer();
	return Play::KeyDown(Play::KeyboardButton::KEY_ESCAPE);
}

// Gets called once when the player quits the game
int MainGameExit( void )
{
	createFile();
	Play::DestroyManager();
	return PLAY_OK;
}

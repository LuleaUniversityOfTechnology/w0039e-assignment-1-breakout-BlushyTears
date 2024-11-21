#pragma once

#define PLAY_USING_GAMEOBJECT_MANAGER

#include "Play.h"
#include <string.h>

void spawnBall();
void StepFrame(float elapsedTime);
void spawnBricks();
void initializePaddle();
void createFile();
void readFile();

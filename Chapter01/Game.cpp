// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"

#define FPS_CAP 60

#define NUM_BALLS 3

const float windowWidth = 1024.0f;
const float windowHeight = 768.0f;

const int thickness = 15;
const float paddleH = 100.0f;

Game::Game()
:mWindow(nullptr)
,mRenderer(nullptr)
,mTicksCount(0)
,mIsRunning(true)
,mPaddleDir(0)
,mPaddle2Dir(0)
{
	
	for (int i = 0; i < NUM_BALLS; i++)
	{
		spawnBall(windowWidth / 2.0f, windowHeight / 2.0f);
	}
}

bool Game::Initialize()
{
	// Initialize SDL
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	
	// Create an SDL Window
	mWindow = SDL_CreateWindow(
		"Game Programming in C++ (Chapter 1)", // Window title
		100,	// Top left x-coordinate of window
		100,	// Top left y-coordinate of window
		1024,	// Width of window
		768,	// Height of window
		0		// Flags (0 for no flags set)
	);

	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}
	
	//// Create SDL renderer
	mRenderer = SDL_CreateRenderer(
		mWindow, // Window to create renderer for
		-1,		 // Usually -1
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}
	mPaddlePos.x = 10.0f;
	mPaddlePos.y = windowHeight/2.0f;
	
	/*mBallPos.x = windowWidth / 2.0f;
	mBallPos.y = windowHeight /2.0f;
	mBallVel.x = -200.0f;
	mBallVel.y = 235.0f;*/
	
	// second player
	mPaddle2Pos.x = windowWidth - 10.0f - thickness;
	mPaddle2Pos.y = windowHeight / 2.0f;

	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			// If we get an SDL_QUIT event, end loop
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}
	
	// Get state of keyboard
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// If escape is pressed, also end loop
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}
	
	// Update paddle direction based on W/S keys
	mPaddleDir = 0;
	if (state[SDL_SCANCODE_W])
	{
		mPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		mPaddleDir += 1;
	}

	// 2nd player
	// Update paddle direction based on W/S keys
	mPaddle2Dir = 0;
	if (state[SDL_SCANCODE_UP])
	{
		mPaddle2Dir -= 1;
	}
	if (state[SDL_SCANCODE_DOWN])
	{
		mPaddle2Dir += 1;
	}
}

void Game::UpdateGame()
{
	// Wait until 16ms (1000/60 = 60 FPS) has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + (1000/FPS_CAP)))
		;

	// Delta time is the difference in ticks from last frame
	// (converted to seconds)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	
	// Clamp maximum delta time value
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	//test
	//std::cout << 1000.0f / deltaTime << "\n";
	//std::cout << mBallPos.y << "\n";

	// Update tick counts (for next frame)
	mTicksCount = SDL_GetTicks();
	
	// Update paddle position based on direction
	if (mPaddleDir != 0)
	{
		mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (mPaddlePos.y < (paddleH/2.0f + thickness))
		{
			mPaddlePos.y = paddleH/2.0f + thickness;
		}
		else if (mPaddlePos.y > (windowHeight - paddleH/2.0f - thickness))
		{
			mPaddlePos.y = windowHeight - paddleH/2.0f - thickness;
		}
	}

	// Extra
	// Update paddle2 position based on direction
	if (mPaddle2Dir != 0)
	{
		mPaddle2Pos.y += mPaddle2Dir * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (mPaddle2Pos.y < (paddleH / 2.0f + thickness))
		{
			mPaddle2Pos.y = paddleH / 2.0f + thickness;
		}
		else if (mPaddle2Pos.y > (windowHeight - paddleH / 2.0f - thickness))
		{
			mPaddle2Pos.y = windowHeight - paddleH / 2.0f - thickness;
		}
	}
	
	for (int i = 0; i < balls.size(); i++)
	{
		balls.at(i).pos.x += balls.at(i).vel.x * deltaTime;
		balls.at(i).pos.y += balls.at(i).vel.y * deltaTime;

		// Bounce if needed
		// Did we intersect with the paddle?
		float diff = mPaddlePos.y - balls.at(i).pos.y;
		float diff2 = mPaddle2Pos.y - balls.at(i).pos.y;
		// Take absolute value of difference
		diff = (diff > 0.0f) ? diff : -diff;
		diff2 = (diff2 > 0.0f) ? diff2 : -diff2;
		if (
			// Our y-difference is small enough
			diff <= paddleH / 2.0f &&
			// We are in the correct x-position
			balls.at(i).pos.x <= 25.0f && balls.at(i).pos.x >= 20.0f &&
			// The ball is moving to the left
			balls.at(i).vel.x < 0.0f)
		{
			if (balls.at(i).pos.y)
				balls.at(i).vel.x *= -1.0f;
		}
		// Did we intersect with the paddle2?
		else if (
			// Our y-difference is small enough
			diff2 <= paddleH / 2.0f &&
			// We are in the correct x-position
			balls.at(i).pos.x >= windowWidth - 25.0f && balls.at(i).pos.x <= windowWidth - 20.0f &&
			// The ball is moving to the left
			balls.at(i).vel.x > 0.0f)
		{
			if (balls.at(i).pos.y)
				balls.at(i).vel.x *= -1.0f;
		}
		// Did the ball go off the screen? (if so, end game)
		else if (balls.at(i).pos.x <= 0.0f || balls.at(i).pos.x >= windowWidth - thickness)
		{
			balls.erase(balls.begin() + i);
			if (balls.size() == 0)
			{
				mIsRunning = false;
				return;
			}
			continue;
		}

		// Did the ball collide with the top wall?
		if (balls.at(i).pos.y <= thickness && balls.at(i).vel.y < 0.0f)
		{
			balls.at(i).vel.y *= -1;
		}
		// Did the ball collide with the bottom wall?
		else if (balls.at(i).pos.y >= (768 - thickness) &&
			balls.at(i).vel.y > 0.0f)
		{
			balls.at(i).vel.y *= -1;
		}
	}
}

void Game::GenerateOutput()
{
	// Set draw color to blue
	SDL_SetRenderDrawColor(
		mRenderer,
		0,		// R
		0,		// G 
		255,	// B
		255		// A
	);
	
	// Clear back buffer
	SDL_RenderClear(mRenderer);

	// Draw walls
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
	
	// Draw top wall
	SDL_Rect wall{
		0,			// Top left x
		0,			// Top left y
		1024,		// Width
		thickness	// Height
	};
	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw bottom wall
	wall.y = 768 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw paddle
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	SDL_Rect paddle{
		static_cast<int>(mPaddlePos.x),
		static_cast<int>(mPaddlePos.y - paddleH/2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &paddle);

	// Extra
	// Draw paddle2
	SDL_Rect paddle2{
		static_cast<int>(mPaddle2Pos.x),
		static_cast<int>(mPaddle2Pos.y - paddleH / 2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &paddle2);
	
	// Draw ball
	SDL_Rect ball{
		0,
		0,
		thickness,
		thickness
	};

	// Extra
	for (int i = 0; i < balls.size(); i++)
	{

		ball.x = static_cast<int>(balls.at(i).pos.x - thickness / 2);
		ball.y = static_cast<int>(balls.at(i).pos.y - thickness / 2);

		SDL_RenderFillRect(mRenderer, &ball);
	}
	
	
	// Swap front buffer and back buffer
	SDL_RenderPresent(mRenderer);
}

void Game::spawnBall(float xPos, float yPos)
{
	Ball ball = { {xPos, yPos}, {200.0f * (rand() % 2 == 0 ? 1 : -1), (rand() % 100) - 100}};
	balls.push_back(ball);
}

void Game::spawnBall(float xPos, float yPos, float xVel, float yVel)
{
	Ball ball = { {xPos, yPos}, {xVel, yVel} };
	balls.push_back(ball);
}

void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

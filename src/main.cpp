#include "raylib.h"
#if defined(PLATFORM_WEB)
	#include "emscripten/emscripten.h"
#endif
#include <cstdlib>
#include "generator.hpp"
#include "maze.hpp"
#include <iostream>
#include <cmath>
#include <string>
#include <algorithm>
#include <sstream>
#include "solver.hpp"

void GameLoop();
void get_waypoint(const Vector2&, Vector2&, Pos&);
void display_options(const std::string&, int, int);

// Not static because it is accessed in another file
int width, height;

// Using global variables because of [GameLoop] function
// Unecessary both web build requires such a function

static constexpr int blockSize = 25;
static int w, h,
		alg = -1, // index of algorithm running
		stepTime_ms = 100; // time to next animation

static float timer = 0, animTimer = 0;

// Maze object
static Maze maze;
// User has dropped 0: start point; 1: end point
static bool waypointsDropped[2] {false, false};
static Vector2 waypoints[2] {}; // Position in world space of the waypoints 
static Pos waypointsPos[2] {}; // Position in [Maze] of the waypoints

int main(int argc, char **argv)
{
	#if !defined(PLATFORM_WEB)
		w = (argc > 1) ? std::atoi(argv[1]) : 40;
		h = (argc > 2) ? std::atoi(argv[2]) : 30;
	// Make it full screen in web browser
	//  by using browser's width
	#else
		w = EM_ASM_INT({ return window.innerWidth; });
		h = EM_ASM_INT({ return window.innerHeight; });

		w /= blockSize;
		h /= blockSize;
	#endif

	width = w * blockSize;
	height = h * blockSize;

	InitWindow(width, height, "Maze Solver");
	SetTargetFPS(60);

	maze = generate_maze(w, h - 1);
	
	// Accommodate both web and desktop
	#if defined(PLATFORM_WEB)
		emscripten_set_main_loop(GameLoop, 0, 1);
	#else
	while(!WindowShouldClose())
		GameLoop();
	#endif

	free_maze(); // Free memory for maze
	CloseWindow();
	
	return 0;
}

void GameLoop()
{
	Vector2 mousePos = GetMousePosition();

	// No algorithm is running
	// Allow to place waypoints and start new algorithms
	if (alg == -1)
	{
		// Allow the user to drop waypoints
		// Left and right mouse buttons
		for (int i = 0; i <= 1; i++)
		{
			// 0: Left mouse button, 1: right mouse button
			if (IsMouseButtonPressed(i))
			{
				// Get waypoint from mouse position
				get_waypoint(mousePos, waypoints[i], waypointsPos[i]);
				waypointsDropped[i] = true;
				// If sorting occurred previously, clear drawn boxes
				clear_boxes();
			}
		}

		// If a start and end point has been specified
		if (waypointsDropped[0] && waypointsDropped[1])
		{
			// Number keys 1 - 3
			for (int i = 0; i < 3; i++)
			{
				if (IsKeyPressed(KEY_ONE + i))
				{
					// Reset timers
					timer = animTimer = 0;
					// Set to specified algorithm
					alg = i;
					break;
				}
			}
		}
	}

	// If the algorithm is running
	// Allow to increase step time
	else
	{
		if (IsKeyPressed(KEY_UP))
			stepTime_ms += 10;
		else if (IsKeyPressed(KEY_DOWN) && stepTime_ms > 10)
				stepTime_ms -= 10;

		float deltaTime = GetFrameTime();
		// Adding up time since last frame gives the amount of time that has passed
		timer += deltaTime, animTimer += deltaTime;
		// This is what gives the pause in visualization
		if (animTimer >= (stepTime_ms / 1000.0f))
		{
			animTimer = 0;
			// After each run of the search algorithm, check if it has ben completed
			bool done = find_path(maze, waypointsPos[0], waypointsPos[1], alg);
			if (done)
				alg = -1; // False value
		} 
	}	

	BeginDrawing();

		ClearBackground(BLACK);

		draw_maze(w, h - 1, blockSize);

		// Not searching
		if (alg == -1)
		{
			// Tell the user how to drop waypoints, if none have been dropped
			if (!waypointsDropped[0] || !waypointsDropped[1])
			{
				DrawText("Left click to place [start].", 5, height - 20, 15, GRAY);
				DrawText("Right click to place [end].", width - 185, height - 20, 15, GRAY);
			}

			// When the user has dropped both waypoints, display algorithm choices
			if (waypointsDropped[0] && waypointsDropped[1])
				display_options("Depth-First Search;Breadth-First Search;A* Pathfinding",
					width / (3 + 1), height - 20);
		}

		// When searching, displaying animation delay
		else
		{
			DrawText(TextFormat("Step Time: %d ms", stepTime_ms), width - 130, height - 20, 15, GRAY);
		}

		// Show timer after user has dropped both waypoints
		// Timer stays even after algorithm has completed for recording purposes
		if (waypointsDropped[0] && waypointsDropped[1])
			DrawText(TextFormat("%.2fs", timer), 5, height- 20, 15, RAYWHITE);

		// Show descriptive boxes and text (for algorithms)
		draw_box(blockSize);

		// Shows user-selected waypoints (on top of everything)
		if (waypointsDropped[0])
			DrawCircleV(waypoints[0], blockSize / 3, BLUE);
		if (waypointsDropped[1])
			DrawCircleV(waypoints[1], blockSize / 3, GOLD);

	EndDrawing();
}

// Set position of waypoint in world space and in Maze based on mouse position
// Ensure waypoint is a multiple of [blockSize], i.e., snap it to grid created by the maze
void get_waypoint(const Vector2& mousePos, Vector2& waypoint, Pos& waypointPos)
{
	// Make position multiple of block size
	waypoint = {std::floorf(mousePos.x / blockSize) * blockSize,
				std::floorf(mousePos.y / blockSize) * blockSize};
	// Get index of position in grid using [blockSize]
	waypointPos = {static_cast<int>(waypoint.x / blockSize),
					static_cast<int>(waypoint.y / blockSize)};
	
	// Circle is centred at (0, 0), so it'll appear at the top-left
	// Move it to the right and down by half the block size, so it can be centred in box
	waypoint.x += (blockSize / 2);
	waypoint.y += (blockSize / 2);

	// Don't allow placing at below certain point
	if (waypointPos.y >= height - 1)
	{
		waypointPos.y--;
		waypoint.y -= blockSize;
	}
}

// Given a string of semicolon-separated phrases
// Display the phrases with an index and a box around that index
void display_options(const std::string& options, int w, int h)
{
	int i = 1;
	// Turn string to input stream
	std::istringstream iss {options};
	std::string option {}; // Keeps track of current string
	// Get string preceding semicolon
	while (std::getline(iss, option, ';'))
	{
		// New text: [i] [string]
		const char *text = TextFormat("%d %s", i++, option.c_str());
		DrawRectangleLines(w - 3, h - 1, 12, 15, GRAY);
		// Show text
		DrawText(text, w, h, 15, RAYWHITE);
		// Shift cursor by width of text + some space
		w += MeasureText(text, 15) + 15;
	}
}

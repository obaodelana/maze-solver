#include "raylib.h"
#include <cstdlib>
#include "generator.hpp"
#include "maze.hpp"
#include <iostream>
#include <cmath>
#include <string>
#include <algorithm>
#include <sstream>
#include "solver.hpp"

void get_endpoint(const Vector2&, Vector2&, Pos&);
void display_options(const std::string&, int, int);

constexpr int blockSize = 25;
int width, height;

int main(int argc, char **argv)
{
	constexpr int noOfAlgs = 3;
	constexpr int alg_keys[] {KEY_ONE, KEY_TWO, KEY_THREE};

	const int w = (argc > 1) ? std::atoi(argv[1]) : 40;
	const int h = (argc > 2) ? std::atoi(argv[2]) : 30;

	width = w * blockSize;
	height = h * blockSize;

	InitWindow(width, height, "Maze Solver");
	SetTargetFPS(60);

	const Maze maze = generate_maze(w, h - 1);
	
	int alg = -1;
	float time = 0, stepTime = 1;

	bool endPointsDropped[2] {false, false};
	Vector2 endPoints[2] {};
	Pos endPointsPos[2] {};

	while(!WindowShouldClose())
	{
		Vector2 mousePos = GetMousePosition();

		if (alg == -1)
		{
			for (int i = 0; i <= 1; i++)
			{
				if (IsMouseButtonPressed(i))
				{
					endPointsDropped[i] = true;
					get_endpoint(mousePos, endPoints[i], endPointsPos[i]);
				}
			}

			for (int i = 0; i < noOfAlgs; i++)
			{
				if (IsKeyPressed(alg_keys[i]))
				{
					time = 0;
					alg = i;
					break;
				}
			}
		}
	
		else
		{
			time += GetFrameTime();
			if (time >= stepTime)
			{
				time = 0;
				bool done = find_path(maze, endPointsPos[0], endPointsPos[1], alg);
				if (done)
				{
					alg = -1;
					endPointsDropped[0] = false, endPointsDropped[1] = false;
				}
			} 
		}	

		BeginDrawing();

			ClearBackground(BLACK);

			draw_maze(w, h - 1, blockSize);

			if (endPointsDropped[0])
				DrawCircleV(endPoints[0], blockSize / 3, GREEN);
			if (endPointsDropped[1])
				DrawCircleV(endPoints[1], blockSize / 3, GOLD);

			if (alg == -1)
			{
				if (!endPointsDropped[0] || !endPointsDropped[1])
				{
					DrawText("Left click to place [start].", 5, height - 20, 15, RAYWHITE);
					DrawText("Right click to place [end].", width - 185, height - 20, 15, RAYWHITE);
				}
				if (endPointsDropped[0] && endPointsDropped[1])
					display_options("Depth-First Search;Breadth-First Search;Dijkstra's Algorithm",
						width / (3 + 1), height - 20);
			}

			else
			{
				draw_box(blockSize / 1.5);
			}

		EndDrawing();
	}

	free_maze();
	CloseWindow();
	
	return 0;
}

void get_endpoint(const Vector2& mousePos, Vector2& endpoint, Pos& endpointPos)
{
	endpoint = {std::floorf(mousePos.x / blockSize) * blockSize,
				std::floorf(mousePos.y / blockSize) * blockSize};
	endpointPos = {static_cast<int>(endpoint.x / blockSize), static_cast<int>(endpoint.y / blockSize)};
	
	endpoint.x += (blockSize / 2);
	endpoint.y += (blockSize / 2);

	if (endpointPos.y >= height - 1)
	{
		endpointPos.y--;
		endpoint.y -= blockSize;
	}
			
}

void display_options(const std::string& options, int w, int h)
{
	int i = 1;
	std::istringstream iss {options};
	std::string option {};
	while (std::getline(iss, option, ';'))
	{
		option = std::to_string(i++) + " " + option;

		DrawRectangleLines(w - 3, h - 1, 12, 15, GRAY);
		DrawText(option.c_str(), w, h, 15, RAYWHITE);
		w += MeasureText(option.c_str(), 15) + 15;
	}
}

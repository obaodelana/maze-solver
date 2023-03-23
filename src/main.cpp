#include "raylib.h"
#include <cstdlib>
#include "generator.hpp"
#include "maze.hpp"
#include <iostream>
#include <cmath>
#include <string>
#include <algorithm>
#include <sstream>

#define OFFWHITE (Color) {247, 240, 240, 255}

void get_endpoint(const Vector2&, Vector2&, Pos&);
void display_options(const std::string&, int, int);

constexpr int blockSize = 25;

int main(int argc, char **argv)
{
	const int w = (argc > 1) ? std::atoi(argv[1]) : 40;
	const int h = (argc > 2) ? std::atoi(argv[2]) : 30;

	const int width = w * blockSize;
	const int height = h * blockSize;

	InitWindow(width, height, "Maze Solver");
	SetTargetFPS(60);

	generate_maze(w, h - 1);

	bool endPointsDropped[2] {false, false};
	Vector2 endPoints[2] {};
	Pos endPointsPos[2] {};

	while(!WindowShouldClose())
	{
		Vector2 mousePos = GetMousePosition();

		if (IsMouseButtonPressed(0))
		{
			endPointsDropped[0] = true;
			get_endpoint(mousePos, endPoints[0], endPointsPos[0]);
			if (endPointsPos[0].y >= h - 1)
			{
				endPointsPos[0].y--;
				endPoints[0].y -= blockSize;
			}
		}

		else if (IsMouseButtonPressed(1))
		{
			endPointsDropped[1] = true;
			get_endpoint(mousePos, endPoints[1], endPointsPos[1]);
			if (endPointsPos[1].y >= h - 1)
			{
				endPointsPos[1].y--;
				endPoints[1].y -= blockSize;
			}
		}

		BeginDrawing();

			ClearBackground(BLACK);

			draw_maze(w, h - 1, blockSize);

			if (!endPointsDropped[0] || !endPointsDropped[1])
				DrawCircle(mousePos.x - 0.1f, mousePos.y + 0.1f, blockSize / 3, OFFWHITE);
			if (endPointsDropped[0])
				DrawCircleV(endPoints[0], blockSize / 3, GREEN);
			if (endPointsDropped[1])
				DrawCircleV(endPoints[1], blockSize / 3, GOLD);

			if (endPointsDropped[0] && endPointsDropped[1])
				display_options("Depth-First Search;Breadth-First Search;Dijkstra's Algorithm",
					width / (3 + 1), height - 20);

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
		DrawText(option.c_str(), w, h, 15, OFFWHITE);
		w += MeasureText(option.c_str(), 15) + 15;
	}
}

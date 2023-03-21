#include "raylib.h"
#include <cstdlib>
#include "generator.hpp"

int main(int argc, char **argv)
{
	const int w = (argc > 1) ? std::atoi(argv[1]) : 40;
	const int h = (argc > 2) ? std::atoi(argv[2]) : 30;
	constexpr int blockSize = 25;

	InitWindow(w * blockSize, h * blockSize, "Maze Solver");
	SetTargetFPS(60);

	generate_maze(w, h);

	while(!WindowShouldClose())
	{
		BeginDrawing();

			draw_maze(w, h, blockSize);

		EndDrawing();
	}

	free_maze();
	CloseWindow();
	
	return 0;
}
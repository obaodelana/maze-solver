#include "raylib.h"
#include <cstdlib>

int main(int argc, char **argv)
{
	const int w = (argc > 1) ? std::atoi(argv[1]) : 40;
	const int h = (argc > 2) ? std::atoi(argv[2]) : 30;
	constexpr int blockSize = 20;

	InitWindow(w * blockSize, h * blockSize, "Maze Solver");
	SetTargetFPS(60);

	while(!WindowShouldClose())
	{

		BeginDrawing();

		

		EndDrawing();
	}

	CloseWindow();
	
	return 0;
}
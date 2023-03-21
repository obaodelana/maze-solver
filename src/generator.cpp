#include "generator.hpp"
#include "maze.hpp"
#include <ctime>
#include <cstdlib>
#include <stack>
#include "raylib.h"

Maze *maze = nullptr;

// Generate a maze with randomized DFS
void generate_maze(int w, int h)
{
	if (maze != nullptr)
		return;

	std::srand(std::time(NULL));

	maze = new Maze(w, h);
	
	// Vertices we've accessed at any point
	std::unordered_set<Pos> explored {}; 
	// Stack used instead of recursion
	std::stack<Pos> nexts {};

	// We've seen the first vertex
	explored.insert({0, 0});
	nexts.push(*explored.begin());
	while (!nexts.empty())
	{
		Pos curr = nexts.top();
		nexts.pop();

		std::vector<Pos> walls = maze->walls(curr), unvisited {};
		// Copy only walls to [unvisited] that have not been visited
		//  and are not at the edge of the screen
		std::copy_if(walls.begin(), walls.end(), std::back_inserter(unvisited),
			[explored, w, h](Pos p) { return (explored.find(p) == explored.end()); });

		// If they are walls that have not been visited	
		if (unvisited.size() > 0)
		{
			// Check current vertex again (sometime later)
			nexts.push(curr);

			// Pick a random vertex
			Pos randomWall = unvisited[std::rand() % unvisited.size()];
			// Remove wall (edge) connecting the two vertices
			maze->remove_wall(curr, randomWall);

			// I've already seen the above vertex
			explored.insert(randomWall);
			// Look at it next
			nexts.push(randomWall);
		}
	}
}

// Use DrawLines to connect vertices (an edge)
void draw_maze(int w, int h, int blockSize)
{
	if (maze == nullptr)
		return;

	for (int i = 0; i <= h; i++)
	{
		for (int j = 0; j <= w; j++)
		{
			Pos pos = {j, i};
			if (!maze->is_vertex(pos))
				continue;

			Vector2 start = {float(pos.x) * blockSize, float(pos.y) * blockSize};
			for (Pos p : maze->walls(pos))
			{
				Vector2 end = {float(p.x) * blockSize, float(p.y) * blockSize};
				DrawLineV(start, end, WHITE);
			}
		}
	}
}

void free_maze()
{
	if (maze != nullptr)
		delete maze;
}

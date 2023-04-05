#include "generator.hpp"
#include "maze.hpp"
#include <ctime>
#include <cstdlib>
#include <stack>
#include "raylib.h"

Maze *maze = nullptr;

// Generate a maze with randomized DFS
/*
	Algorithm starts at a given vertex, and randomly picks only a single neighbour
		that has not been looked at before (it doesn't look at *each* neighbour).
	Then it removes the edge connecting the vertex to its neighbour.
	It then looks at the chosen (just removed) neighbour and add the current vertex again
		(so it can remove another neighbour if it has the opportunity)

	In this way, it ends up removing one edge from each visited vertex.
	In our case, a vertex has 4 or less neighbours, so when the algorithm runs,
		it removes an edge from a vertex and marks the vertex as visited, therefore,
		we can't remove an edge containing this vertex again.
	This is why this algorithm doesn't just remove all edges

	Why it creates a maze pattern is still unknown to me
	- Oba
*/
const Maze& generate_maze(int w, int h)
{
	if (maze != nullptr)
		return *maze;

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

		// Holds neighbouring vertices that have not been visited
		std::vector<Pos> unvisited = maze->walls(curr);
		// Remove vertices that have been visited
		unvisited.erase(
			std::remove_if(unvisited.begin(), unvisited.end(),
				[explored](Pos p) { return (explored.find(p) != explored.end()); }),
			unvisited.end()
		);

		// If they are walls that have not been visited	
		if (!unvisited.empty())
		{
			// Check current vertex again (sometime later)
			nexts.push(curr);

			// Pick a random vertex from [unvisited]
			Pos randomVert = unvisited[std::rand() % unvisited.size()];
			// Remove wall (edge) connecting the two vertices
			maze->remove_wall(curr, randomVert);

			// I've already seen the above vertex
			explored.insert(randomVert);
			// Look at it next
			nexts.push(randomVert);
		}
	}

	return *maze;
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
			// i represents the current row
			// j represents the current column
			Pos pos = {j, i};
			if (!maze->is_vertex(pos))
				continue;

			// Starting point
			Vector2 start = {float(pos.x) * blockSize, float(pos.y) * blockSize};
			for (Pos p : maze->walls(pos))
			{
				// Ending point
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

// Use an adjaceny list to represent graph
// Removed edges represents removed walls
// Use lines to represent edges
// generate() returns a graph with disconnected edges
// draw draws the graph with line segmenents of width 1

#include "maze.hpp"
#include <cassert>

Maze::Maze(size_t row, size_t col)
{
	auto validEdge = [row, col](Pos edge)
	{
		return edge.x >= 0 && edge.x < col
			&& edge.y >= 0 && edge.y < row;
	};

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			const Pos edges[]
			{
				{i - 1, j},
				{i + 1, j},
				{i, j - 1},
				{i, j + 1}
			};

			for (Pos e : edges)
			{
				if (validEdge(e))
					maze[{i, j}].insert(e);
			}
		}
	}
}

bool Maze::is_vertex(const Pos& vertex) const
{
	return maze.find(vertex) != maze.end();
}

bool Maze::is_wall(const Pos& vertex, const Pos& wall)
{
	return (is_vertex(vertex) &&
		maze[vertex].find(wall) != maze[vertex].end());
}

size_t Maze::num_of_neighbours(const Pos& vertex)
{
	if (is_vertex(vertex))
		return maze[vertex].size();
	return 0;
}

std::unordered_set<Pos>::const_iterator Maze::neighbours(const Pos& vertex)
{
	assert(is_vertex(vertex) && "maze.cpp: Vertex doesn't exist!");

	return maze[vertex].cbegin();
}

std::unordered_set<Pos>::const_iterator Maze::lastNeighbour(const Pos& vertex)
{
	assert(is_vertex(vertex) && "maze.cpp: Vertex doesn't exist!");
	
	return maze[vertex].cend();
}

// Returns true if wall is successfully removed
bool Maze::remove_wall(const Pos& vertex, const Pos& wall)
{
	if (is_wall(vertex, wall))
	{
		maze[vertex].erase(wall);
		return true;
	}

	return false;
}

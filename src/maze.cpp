#include "maze.hpp"
#include <cassert>
#include <algorithm>

Maze::Maze(size_t col, size_t row) : col(col), row(row)
{	
	for (int i = 0; i <= row; i++)
	{
		for (int j = 0; j <= col; j++)
		{
			const Pos edges[]
			{
				{j - 1, i},
				{j + 1, i},
				{j, i - 1},
				{j, i + 1}
			};

			for (Pos e : edges)
			{
				if (validEdge(e))
					maze[{j, i}].insert(e);
			}
		}
	}
}

std::ostream& operator<<(std::ostream& out, const Pos& pos)
{
	out << pos.x << ", " << pos.y;
	return out;
}

bool Maze::validEdge(const Pos& edge) const
{
	return edge.x >= 0 && edge.x <= col
		&& edge.y >= 0 && edge.y <= row;
}

bool Maze::is_vertex(const Pos& vertex) const
{
	return maze.find(vertex) != maze.end();
}

bool Maze::is_wall(const Pos& vertex, const Pos& wall) const
{
	return (is_vertex(vertex) &&
		maze.at(vertex).find(wall) != maze.at(vertex).end());
}

size_t Maze::num_of_neighbours(const Pos& vertex) const
{
	if (is_vertex(vertex))
		return maze.at(vertex).size();
	return 0;
}

std::vector<Pos> Maze::walls(const Pos& vertex) const
{
	std::vector<Pos> verts {};

	if (is_vertex(vertex))
	{
		for (auto n : maze.at(vertex))
			verts.push_back(n);
	}

	return verts;
}

std::vector<Pos> Maze::paths(const Pos& vertex) const
{
	std::vector<Pos> openPaths {
		{vertex.x, vertex.y - 1},
		{vertex.x, vertex.y + 1},
		{vertex.x - 1, vertex.y},
		{vertex.x + 1, vertex.y}
	};

	openPaths.erase
	(
		std::remove_if(openPaths.begin(), openPaths.end(), [this, vertex](const Pos& pos)
			{ return !this->validEdge(pos) || is_wall(vertex, pos); }),
		openPaths.end()
	);

	return openPaths;
}

// Returns true if wall is successfully removed
bool Maze::remove_wall(const Pos& vertex, const Pos& wall)
{
	if (is_wall(vertex, wall))
	{
		maze[vertex].erase(wall);
		if (is_wall(wall, vertex))
			maze[wall].erase(vertex);

		return true;
	}

	return false;
}

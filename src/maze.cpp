#include "maze.hpp"
#include <cassert>
#include <algorithm>
#include <iostream>

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
				if (is_valid_vertex(e))
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

bool Maze::is_valid_vertex(const Pos& vertex) const
{
	return vertex.x >= 0 && vertex.x <= col
		&& vertex.y >= 0 && vertex.y <= row;
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
	std::vector<Pos> openPaths {};

	Pos vertexRight = {vertex.x + 1, vertex.y},
		vertexDown = {vertex.x, vertex.y + 1};
	Pos oppVertex = {vertex.x + 1, vertex.y + 1};

	if (!is_vertex(vertex) || !is_vertex(vertexRight)
		|| !is_vertex(vertexDown) || !is_vertex(oppVertex))
		return openPaths;

	if (!is_wall(vertex, vertexRight))
	{
		Pos vertexUp = {vertex.x, vertex.y - 1};
		if (is_vertex(vertexUp))
			openPaths.push_back(vertexUp);
	}

	if (!is_wall(oppVertex, vertexDown))
	{
		if (is_vertex(vertexDown))
			openPaths.push_back(vertexDown);
	}

	if (!is_wall(vertex, vertexDown))
	{
		Pos vertexLeft = {vertex.x - 1, vertex.y};
		if (is_vertex(vertexLeft))
			openPaths.push_back(vertexLeft);
	}

	if (!is_wall(oppVertex, vertexRight))
	{
		if (is_vertex(vertexRight))
			openPaths.push_back(vertexRight);
	}

	// We don't want extreme-end paths
	openPaths.erase(std::remove_if(
		openPaths.begin(), openPaths.end(),
		[this](Pos p){ return p.x >= this->width() || p.y >= this->height(); }
	), openPaths.end());

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

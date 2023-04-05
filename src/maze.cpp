#include "maze.hpp"
#include <cassert>
#include <algorithm>
#include <iostream>

// Create specified number of nodes and define edges
Maze::Maze(size_t col, size_t row) : col(col), row(row)
{	
	auto is_valid = [col, row](const Pos& v)
	{
		return v.x >= 0 && v.x <= col 
			&& v.y >= 0 && v.y <= row;
	};

	for (int i = 0; i <= row; i++)
	{
		for (int j = 0; j <= col; j++)
		{
			// Possible neighbours are vertices
			const Pos edges[]
			{
				{j - 1, i}, // above
				{j + 1, i}, // below
				{j, i - 1}, // to the left
				{j, i + 1} // to the right
			};

			// Add only valid vertices
			for (Pos e : edges)
			{
				if (is_valid(e))
					maze[{j, i}].insert(e);
			}
		}
	}
}

// Print out [Pos] nicely
std::ostream& operator<<(std::ostream& out, const Pos& pos)
{
	out << pos.x << ", " << pos.y;
	return out;
}

bool Maze::is_vertex(const Pos& vertex) const
{
	return maze.find(vertex) != maze.end();
}

/* .at() is used for const objects, because operator[] has
	no const definition 
*/

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

// Returning neighbours
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

// Return movable vertices
/*
	This is a bit tricky cause we can't only consider a vertices neighbours
		because [Pos] is actually the intersection of vertices not the space of the box.
		--+--+--
		  |  |
		--+--+--
		  |  |

		The plus sign (+) is [Pos], so examining its neighbours only won't give the pathways.
		What I do, is check
			- if a vertex is not connected to it's right neighbour, then we can move up
			- if a vertex is not connected to it's bottom neighbour, then we can move left
			- if the diagonal the of the vertex (i.e. bottom right neighbour) is
				- not connected to it's top neighbour, then we can move right
				- not connected to it's left neighbour, then we can move down
*/
std::vector<Pos> Maze::paths(const Pos& vertex) const
{
	std::vector<Pos> openPaths {};

	// Right and bottom neighbours
	Pos vertexRight = {vertex.x + 1, vertex.y},
		vertexDown = {vertex.x, vertex.y + 1};
	// Right bottom neighbour
	Pos oppVertex = {vertex.x + 1, vertex.y + 1};

	// If all of them are not neighbours, then this guy is isolated
	if (!is_vertex(vertex) || !is_vertex(vertexRight)
		|| !is_vertex(vertexDown) || !is_vertex(oppVertex))
		return openPaths;

	// If the vertex is not connected to it's right neighbour, then we can move up
	if (!is_wall(vertex, vertexRight))
	{
		Pos vertexUp = {vertex.x, vertex.y - 1};
		if (is_vertex(vertexUp))
			openPaths.push_back(vertexUp);
	}

	// If the opposite vertex is not connected to it's left neighbour,
	// 	i.e, the current vertex's bottom neighbour, then we can move down
	if (!is_wall(oppVertex, vertexDown))
	{
		if (is_vertex(vertexDown))
			openPaths.push_back(vertexDown);
	}

	// If the vertex is not connected to it's bottom neighbour, then we can move left
	if (!is_wall(vertex, vertexDown))
	{
		Pos vertexLeft = {vertex.x - 1, vertex.y};
		if (is_vertex(vertexLeft))
			openPaths.push_back(vertexLeft);
	}

	// If the opposite vertex is not connected to it's top neighbour,
	// 	i.e, the current vertex's right neighbour, then we can move right
	if (!is_wall(oppVertex, vertexRight))
	{
		if (is_vertex(vertexRight))
			openPaths.push_back(vertexRight);
	}

	// We don't want extreme-end paths, i.e, paths that are outside the screen
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
		// Erase both ways (since it's an undirected graph)
		maze[vertex].erase(wall);
		if (is_wall(wall, vertex))
			maze[wall].erase(vertex);

		return true;
	}

	return false;
}

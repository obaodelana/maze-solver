
#ifndef MAZE_H_
#define MAZE_H_

#include <unordered_map>
#include <unordered_set>
#include <cstddef>
#include <vector>
#include "raylib.h"
#include <ostream>
#include <cmath>

// Node / vertex of undirected graph
struct Pos
{
	int x, y;
	Pos() : x(0), y(0) {}
	Pos(int x, int y): x(x), y(y) {}

	// Equal to
	bool operator==(const Pos& other) const
		{ return x == other.x && y == other.y; }
	// Not equal to
	bool operator!=(const Pos& other) const
		{ return !(*this == other); }
	// Manhattan distance between two nodes
	int distance(const Pos& other) const
		{ return std::abs(x - other.x) + std::abs(y - other.y); }

	// Define sending to output stream
	friend std::ostream& operator<<(std::ostream&, const Pos&);
};

namespace std
{
	template <>
	// Define custom hashing function for [Pos] struct, so it can be used as a key
	struct hash<Pos>
	{
		// Hash function for [Pos] is the EXOR of the its two values (not the best)
		size_t operator() (const Pos& pos) const
		{
			using std::size_t;
			using std::hash;

			return (hash<int>()(pos.x) ^ (hash<int>()(pos.y) << 1));
		}
	};
}

class Maze
{
private:
	size_t row = 0, col = 0;
	// Graph as adjacency list
	std::unordered_map<Pos, std::unordered_set<Pos>> maze;

public:
	// Default constructor
	Maze(size_t, size_t);

	// Remove edge connecting two vertices (both ways)
	bool remove_wall(const Pos&, const Pos&);
	
	// Return list of neighbours
	std::vector<Pos> walls(const Pos&) const;
	// Return neighbouring vertices that are not blocked off by walls
	std::vector<Pos> paths(const Pos&) const;

	bool is_vertex(const Pos&) const;
	bool is_wall(const Pos&, const Pos&) const; 

	size_t width() const { return col; }
	size_t height() const { return row; }
	size_t num_of_neighbours(const Pos& vertex) const;
};

#endif

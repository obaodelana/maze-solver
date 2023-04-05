
#ifndef MAZE_H_
#define MAZE_H_

#include <unordered_map>
#include <unordered_set>
#include <cstddef>
#include <vector>
#include "raylib.h"
#include <ostream>
#include <cmath>

struct Pos
{
	int x, y;
	Pos() : x(0), y(0) {}
	Pos(int x, int y): x(x), y(y) {}

	bool operator==(const Pos& other) const
	{
		return x == other.x && y == other.y;
	}

	bool operator!=(const Pos& other) const
	{
		return !(*this == other);
	}

	int distance(const Pos& other) const
	{
		return std::powf(x - other.x, 2) + std::powf(y - other.y, 2);
	}

	friend std::ostream& operator<<(std::ostream&, const Pos&);
};

namespace std
{
	template <>
	// Define custom hashing function for Pos struct so can be a key in a map or set
	struct hash<Pos>
	{
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
	std::unordered_map<Pos, std::unordered_set<Pos>> maze;

	bool is_valid_vertex(const Pos&) const;

public:
	Maze(size_t, size_t);

	// Remove edge connecting two vertices
	bool remove_wall(const Pos&, const Pos&);
	
	std::vector<Pos> walls(const Pos&) const;
	// Return neighbours that are not wall
	std::vector<Pos> paths(const Pos&) const;

	bool is_vertex(const Pos&) const;
	bool is_wall(const Pos&, const Pos&) const; 

	size_t width() const { return col; }
	size_t height() const { return row; }
	size_t num_of_neighbours(const Pos& vertex) const;
};

#endif

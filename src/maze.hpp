#include <unordered_map>
#include <unordered_set>
#include <cstddef>

struct Pos
{
	int x, y;
	Pos(int x, int y): x(x), y(y) {}

	bool operator==(const Pos& other) const
	{
		return x == other.x && y == other.y;
	}

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
	std::unordered_map<Pos, std::unordered_set<Pos>> maze;

public:
	Maze(size_t, size_t);

	// Remove wall connecting two vertices
	bool remove_wall(const Pos&, const Pos&);
	
	// Neighbour iteration
	std::unordered_set<Pos>::const_iterator neighbours(const Pos&);
	std::unordered_set<Pos>::const_iterator lastNeighbour(const Pos&);

	bool is_vertex(const Pos&) const;
	bool is_wall(const Pos&, const Pos&); 

	size_t vertices_len() const { return maze.size(); }
	size_t num_of_neighbours(const Pos& vertex);
};
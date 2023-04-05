#include "solver.hpp"
#include "raylib.h"
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <list> // linked list
#include <queue> // prority_queue

extern int width, height;

enum Algorithm : int { DFS = 0, BFS, A_STAR };

// Holds the total cost and distance to goal (for A*)
typedef std::pair<int, int> Cost_Dist;
// Item in priority queue (for A*)
typedef std::pair<Pos, Cost_Dist> A_Pos;

// static here means only declare in one file

// Depth- and breadth-first search algorithms are in one function
//  cause they differ by only one line
static bool dfs_bfs(bool, const Maze&, Pos,
	std::unordered_set<Pos>&, std::list<Pos>&,
	std::unordered_map<Pos, Pos>&);
// Helper function to backtrack shortest path for end to start
static void show_path(const Maze&, Pos, Pos, const std::unordered_map<Pos, Pos>&);

// A* algorithm
template <typename C>
static bool a_star(const Maze&, Pos,
	std::unordered_set<Pos>&,
	std::priority_queue<A_Pos, std::vector<A_Pos>, C>&,
	std::unordered_map<Pos, Cost_Dist>&,
	std::unordered_map<Pos, Pos>&);

/*
	Each search function performs only a step of the entire task.
	The state of the function is preserved in static variables, so
	it can continue from where it stopped when called again.

	This allows the search to be non-blocking.
*/

static bool searching = false, path404 = false;

// Stores colour of box for each [Pos]
static std::unordered_map<Pos, Color> boxes {};
static std::unordered_map<Pos, Cost_Dist> a_star_texts {};

static Color MINT = (Color) {99, 163, 117, 255},
	TEXT_COLOR = (Color) {125, 97, 103, 255}; 

// Comparator for priority queue,
// which returns true when the first guy has a higher cost than the second
// Creates a min-heap priority queue
static auto a_compare = [](const A_Pos& one, const A_Pos& two)
	{ return one.second.first > two.second.first; };

bool find_path(const Maze& maze, Pos start, Pos end, int alg)
{
	// static here means they last for the lifetime of the program
	
	// Stores already explored nodes
	static std::unordered_set<Pos> visited;
	// Frontier for DFS and BFS (used a linked list cause it can act as a stack and queue)
	static std::list<Pos> container;
	// Spanning tree for retrieving shortest path
	static std::unordered_map<Pos, Pos> searchTree;
	// A* data structures
	// Priority queue which returns lowest cost element first
	static std::priority_queue<A_Pos, std::vector<A_Pos>, decltype(a_compare)> a_container (a_compare);
	static std::unordered_map<Pos, Cost_Dist> costs;

	// If this function is called and [searching is false], then we are to set up things for searching
	if (!searching)
	{
		searching = true;
		clear_boxes();

		// Put starting node in correct container
		if (alg != A_STAR)
			container.push_back(start);
		else
		{
			// For A*, we also need to initialize the cost of [start]
			costs[start] = {start.distance(end), 0};
			a_container.push({start, costs[start]});
		}

		searchTree[start] = start;
	}

	// Call algorithm based on index
	switch (alg)
	{
		case DFS: case BFS:
			searching = dfs_bfs(alg == DFS, maze, end, visited, container, searchTree);
			break;

		case A_STAR:
		 	searching = a_star(maze, end, visited, a_container, costs, searchTree);
			break;
		
		// Unknown algorithm
		default:
			std::cerr << "solver.cpp: error: Unkown algorithm\n";
			searching = false;
			return false;
	}

	// If searching has ended, clear variables
	if (!searching)
	{
		visited.clear();
		boxes.clear();

		if (alg != A_STAR)
			container.clear();
		else
		{
			// Priority queue doesn't have clear() function
			while (!a_container.empty())
				a_container.pop();
			costs.clear();
		}

		// Draw out shortest path
		show_path(maze, start, end, searchTree);
		searchTree.clear();
	}

	return !searching;
}

// DFS and BFS algorithms
/*
	We are traversing the graph, while making sure to avoid
		already [visited] nodes.
	The preceding vertex used to get to the next vertex is stored in [searchTree].
	The difference between BFS and DFS is the use of the [container].
	In BFS, we use a queue, so vertex that were added first are explored first,
		while in DFS, we explored the last added vertex, so we go deep inside a
		specific path.
	When the goal is reached or they are no more vertices to explore, we stop
*/
static bool dfs_bfs(
	bool dfs,
	const Maze& maze,
	Pos goal,
	std::unordered_set<Pos>& visited,
	std::list<Pos>& container,
	std::unordered_map<Pos, Pos>& searchTree)
{
	// No path found
	if (container.empty())
		return false;

	// Get next vertex (current)
	Pos curr = container.front();
	container.pop_front();

	// We've found the goal
	if (curr == goal)
		return false;

	// If we've visited this node before,
	//  i.e, it was added again by another vertex before it was explored
	if (visited.find(curr) != visited.end())
	 	// We want to skip it, but for the visualization to be seamless, we go to the next vertex
		return dfs_bfs(dfs, maze, goal, visited, container, searchTree);
	else
		visited.insert(curr);

	// Explored gets green
	boxes[curr] = MINT;
	// Go through possible paths
	for (Pos next : maze.paths(curr))
	{
		// If you haven't been visited
		if (visited.find(next) == visited.end())
		{
			// Stack
			if (dfs)
				container.push_front(next);
			// Queue
			else
				container.push_back(next);

			// Save parent
			searchTree[next] = curr;

			// Unvisited gets light gray
			boxes[next] = LIGHTGRAY;
		}
	}
	
	// True means we are still searching
	return true;
}

// A* algorithm
/*
	DFS and BFS are uninformed because they don't know where the goal is,
		they just mindlessly search around without knowing until the come across the goal.
	However, A* has adds a new cost attribute that is informed by the distance left to the goal
		and how far a vertex has moved from the start.
	This allows the algorithm to make smart choices on which vertex to explore next.
	
	Starting from the starting vertex, we take a look at all its neighbours.
	If a neighbour has not been visited, the cost which is
		the distance from the start to the this node + the estimated distance to the goal
		is computed (the distance from the starting vertex and cost is saved in [costs]).
	If a neighbour has already been visited, we check if the path from the current node
		to it is cheaper than what is already in the queue.
	We then add the neighbour to the priority queue if satisfies any condition above.

	The priority queue returns the cheapest node. Because going back is more expensive 
		than going forward, we don't need to check if a node has been visited when exploring
		(it automatically avoids cycle cause it knows if something is in the queue and ignores it
		if it's more expensive).
	However, a node can be visited twice, if a cheaper path is found through it.

	We continue this until, we reach the goal or nothing else is left.

	A spanning tree ([searchTree]) is used to backtrack to get the shortest path.
*/
template <typename C>
static bool a_star(
	const Maze& maze,
	Pos goal,
	std::unordered_set<Pos>& visited,
	std::priority_queue<A_Pos, std::vector<A_Pos>, C>& container,
	std::unordered_map<Pos, Cost_Dist>& costs,
	std::unordered_map<Pos, Pos>& searchTree)
{
	// No path found
	if (container.empty())
		return false;

	// Get least expensive element from priority queue
	A_Pos curr = container.top();
	container.pop();

	// Element returned from queue is {[Pos], {cost, "distance from start"}}
	Pos currPos = curr.first;
	int distFromStart = curr.second.second;

	// Explored
	visited.insert(currPos);

	// Goal is reached!
	if (currPos == goal)
		return false;

	// Colour
	boxes[currPos] = MINT;
	a_star_texts[currPos] = curr.second;
	// Check open paths
	for (Pos next : maze.paths(currPos))
	{
		// Avoid already seen guys (it'll be more expensive anyway)
		if (visited.find(next) != visited.end())
			continue;

		// Since it's an unweighted graph, the distance to move from one node to another is 1
		int g = distFromStart + 1;
		// Check if current node is in priority queue
		// (everyone in the queue has their cost set)
		bool inContainer = (costs.find(next) != costs.end());
		// If node is not in queue
		if (!inContainer
			// Or it is, but it's cheaper through this node
			//  (i.e, it is closer to the start)
			|| (inContainer && g < costs[next].second))
		{
			// Save the cost and distance to start [g]
			costs[next] = {next.distance(goal) + g, g};
			// Add to queue
			container.push({next, costs[next]});

			// Save parent
			searchTree[next] = currPos;

			boxes[next] = LIGHTGRAY;
			a_star_texts[next] = costs[next];
		}
	}

	// Still searching
	return true;
}

static void show_path(const Maze& maze, Pos start, Pos end, const std::unordered_map<Pos, Pos>& searchTree)
{
	if (searchTree.find(end) == searchTree.end())
	{
		path404 = true;
		return;
	}

	Pos move = end;
	while (move != start)
	{
		// Light green
		boxes[move] = (Color) {122, 229, 130, 255};
		move = searchTree.at(move);
	}
	boxes[start] = (Color) {122, 229, 130, 255};

}

void draw_box(int boxSize)
{
	for (auto& box : boxes)
		DrawRectangle(box.first.x * boxSize, box.first.y * boxSize, boxSize, boxSize, box.second);

	if (path404)
		DrawText("Path Not Found!", width / 2 - 99, height / 2, 25, RED);
}

void clear_boxes()
{
	boxes.clear();
	path404 = false;
}

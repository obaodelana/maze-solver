#include "solver.hpp"
#include "raylib.h"
#include <unordered_set>
#include <unordered_map>
#include <list>
#include <iostream>
#include <queue> // prority_queue

extern int width, height;

enum Algorithm : int { DFS = 0, BFS, A_STAR };

typedef std::pair<int, int> Cost_Dist;
typedef std::pair<Pos, Cost_Dist> A_Pos;

bool dfs_bfs(bool, const Maze&, Pos,
	std::unordered_set<Pos>&, std::list<Pos>&,
	std::unordered_map<Pos, Pos>&);

void show_path(const Maze&, Pos, Pos, const std::unordered_map<Pos, Pos>&);

template <typename C>
bool a_star(const Maze&, Pos,
	std::unordered_set<Pos>&,
	std::priority_queue<A_Pos, std::vector<A_Pos>, C>&,
	std::unordered_map<Pos, Cost_Dist>&,
	std::unordered_map<Pos, Pos>&);

/*
	Each search function performs only a step of the entire task.
	The state of the function is preserved in static variables, so
	it can continue from where it stopped when called again.

	This allows the search to be non-blocking
*/

static bool searching = false, path404 = false;
static std::unordered_map<Pos, Color> boxes {};

static Color MINT = (Color) {99, 163, 117, 255}; 

static auto a_compare = [](const A_Pos& one, const A_Pos& two)
{
	int cost1 = one.second.first, cost2 = two.second.first;
	return cost1 > cost2;
};

bool find_path(const Maze& maze, Pos start, Pos end, int alg)
{
	static std::unordered_set<Pos> visited;
	static std::list<Pos> container;
	static std::unordered_map<Pos, Pos> searchTree;
	// A* data structures
	static std::priority_queue<A_Pos, std::vector<A_Pos>, decltype(a_compare)> a_container (a_compare);
	static std::unordered_map<Pos, Cost_Dist> costs;

	if (!searching)
	{
		searching = true;
		clear_boxes();

		if (alg != A_STAR)
			container.push_back(start);
		else
		{
			costs[start] = {start.distance(end), 0};
			a_container.push({start, costs[start]});
		}

		searchTree[start] = start;
	}

	switch (alg)
	{
		case DFS: case BFS:
			searching = dfs_bfs(alg == DFS, maze, end, visited, container, searchTree);
			break;

		case A_STAR:
		 	searching = a_star(maze, end, visited, a_container, costs, searchTree);
			break;
		
		deault:
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
			while (!a_container.empty())
				a_container.pop();
			costs.clear();
		}

		show_path(maze, start, end, searchTree);
		searchTree.clear();
	}

	return !searching;
}

bool dfs_bfs(bool dfs, const Maze& maze, Pos goal,
	std::unordered_set<Pos>& visited,
	std::list<Pos>& container,
	std::unordered_map<Pos, Pos>& searchTree)
{
	if (container.empty())
		return false;

	Pos curr = container.front();
	container.pop_front();

	// We've found the goal
	if (curr == goal)
		return false;

	if (visited.find(curr) != visited.end())
		return dfs_bfs(dfs, maze, goal, visited, container, searchTree);
	else
		visited.insert(curr);

	// Mint
	boxes[curr] = MINT;
	// Go through neighbours that are not walls
	for (Pos next : maze.paths(curr))
	{
		// If you haven't been visited
		if (visited.find(next) == visited.end())
		{
			if (dfs)
				container.push_front(next);
			else
				container.push_back(next);

			searchTree[next] = curr;

			boxes[next] = LIGHTGRAY;
		}
	}
	
	return true;
}

template <typename C>
bool a_star(const Maze& maze, Pos goal,
	std::unordered_set<Pos>& visited,
	std::priority_queue<A_Pos, std::vector<A_Pos>, C>& container,
	std::unordered_map<Pos, Cost_Dist>& costs,
	std::unordered_map<Pos, Pos>& searchTree)
{
	if (container.empty())
		return false;

	A_Pos curr = container.top();
	container.pop();

	Pos currPos = curr.first;
	int distFromStart = curr.second.second;

	visited.insert(currPos);

	if (currPos == goal)
		return false;

	boxes[currPos] = MINT;
	for (Pos next : maze.paths(currPos))
	{
		if (visited.find(next) != visited.end())
			continue;

		int g = distFromStart + 1;
		bool inContainter = (costs.find(next) != costs.end());
		if (!inContainter
			|| (inContainter && g < costs[next].second))
		{
			costs[next] = {next.distance(goal) + g, g};
			container.push({next, costs[next]});

			searchTree[next] = currPos;

			boxes[next] = LIGHTGRAY;
		}
	}

	return true;
}

void draw_box(int boxSize)
{
	for (auto& box : boxes)
	{
		DrawRectangle(box.first.x * boxSize, box.first.y * boxSize, boxSize, boxSize, box.second);
	}

	if (path404)
		DrawText("Path Not Found!", width / 2 - 99, height / 2, 25, RED);
}

void clear_boxes()
{
	boxes.clear();
	path404 = false;
}

void show_path(const Maze& maze, Pos start, Pos end, const std::unordered_map<Pos, Pos>& searchTree)
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

#include "maze.hpp"
#include "raylib.h"
#include <unordered_set>
#include <unordered_map>
#include <list>
#include <iostream>

extern int width, height;

bool dfs_bfs(bool dfs, const Maze&, Pos,
	std::unordered_set<Pos>&, std::list<Pos>&,
	std::unordered_map<Pos, Pos>&);

void show_path(const Maze&, Pos, Pos,
	const std::unordered_map<Pos, Pos>&);

/*
	Each search function performs only a step of the entire task.
	The state of the function is preserved in static variables, so
	it can continue from where it stopped when called again.

	This allows the search to be non-blocking
*/

bool searching = false, path404 = false;
std::unordered_map<Pos, Color> boxes {};

enum Algorithm : int { DFS = 0, BFS, DIJKSTRA };

bool find_path(const Maze& maze, Pos start, Pos end, int alg)
{
	static std::unordered_set<Pos> visited;
	static std::list<Pos> container;
	static std::unordered_map<Pos, Pos> searchTree;

	if (!searching)
	{
		searching = true;
		path404 = false;
		boxes.clear();

		container.push_back(start);
		searchTree[start] = start;
	}

	switch (alg)
	{
		case DFS: case BFS:
			searching = dfs_bfs(alg == DFS, maze, end, visited, container, searchTree);
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
		container.clear();
		boxes.clear();
		
		show_path(maze, start, end, searchTree);
		searchTree.clear();
	}

	return !searching;
}

bool dfs_bfs(bool dfs, const Maze& maze, Pos goal,
	std::unordered_set<Pos>& visited, std::list<Pos>& container,
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
	boxes[curr] = (Color) {99, 163, 117, 255};
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

#include "maze.hpp"
#include "raylib.h"
#include <unordered_set>
#include <unordered_map>
#include <list>
#include <iostream>

bool dfs(const Maze&, Pos,
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

bool searching = false;
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
		boxes.clear();

		container.push_back(start);
		searchTree[start] = start;
	}

	switch (alg)
	{
		case DFS:
			searching = dfs(maze, end, visited, container, searchTree);
			break;
		
		deault:
			std::cout << "solver.cpp: error: Unkown algorithm\n";
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

bool dfs(const Maze& maze, Pos goal,
	std::unordered_set<Pos>& visited, std::list<Pos>& stack,
	std::unordered_map<Pos, Pos>& searchTree)
{
	if (stack.empty())
		return false;

	Pos curr = stack.front();
	stack.pop_front();

	// We've found the goal
	if (curr == goal)
		return false;

	if (visited.find(curr) != visited.end())
	{
		boxes[curr] = LIME;
		return true;
	}
	
	else
		visited.insert(curr);

	// Light green
	boxes[curr] = (Color) {225, 255, 147, 255};
	// Go through neighbours that are not walls
	for (Pos next : maze.paths(curr))
	{
		// If you haven't been visited
		if (visited.find(next) == visited.end())
		{
			stack.push_front(next);
			searchTree[next] = curr;

			boxes[next] = LIGHTGRAY;
		}
		std::cout << '\n';
	}
	
	return true;
}

void draw_box(int boxSize)
{
	for (auto& box : boxes)
	{
		DrawRectangle(box.first.x * boxSize, box.first.y * boxSize, boxSize, boxSize, box.second);
	}
}

void show_path(const Maze& maze, Pos start, Pos end, const std::unordered_map<Pos, Pos>& searchTree)
{
	// If [end] is not reachable from [start]
	// Look for closest neighbour to [end]
	int r = 0;
	while (searchTree.find(end) == searchTree.end())
	{
		// Create bounding box around [end]
		Pos nearby[] =
		{
			{end.x - r - 1, end.y + r + 1},
			{end.x + r, end.y + r + 1},
			{end.x + r + 1, end.y + r + 1},
			{end.x - r - 1, end.y + r},
			{end.x + r + 1, end.y + r},
			{end.x - r - 1, end.y - r - 1},
			{end.x + r, end.y - r - 1},
			{end.x + r + 1, end.y - r - 1}
		};

		for (Pos n : nearby)
		{
			if (maze.is_vertex(n))
			{
				if (searchTree.find(n) != searchTree.end())
				{
					end = n;
					break;
				}
			}
		}

		r++;
	}
	
	Pos move = end;
	while (move != start)
	{
		boxes[move] = GREEN;
		move = searchTree.at(move);
	}
	boxes[start] = GREEN;
}

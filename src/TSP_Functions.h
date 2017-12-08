#ifndef CS375_PROJECT_SRC_TSP_FUNCTIONS_H
#define CS375_PROJECT_SRC_TSP_FUNCTIONS_H

#include <climits>
#include <iostream>
#include <vector>

/** 
 *	Uses brute force approach to solve TSP.
 *	Params: adj is adjacency matrix, v is number of nodes
 *	Shortest path is stored in shortest_path param
 *	Returns: cost of shortest path
**/
int brute_force(int **adj, int v, int *shortest_path);

/**
 *	Uses nearest neighbor approach (greedy) to solves TSP.
 *	Params: adj is adjacency matrix, v is number of nodes
 *	Approxiamte shortest path is stored in shortest_path param
 *	Returns: cost of approximate shortest path
**/
int nearest_neighbor(int **adj, int v, int *shortest_path);

/** 
 *	Uses best first search approach to solve TSP.
 *	Params: adj is adjacency matrix, v is number of nodes
 *	Shortest path is stored in shortest_path param
 *	Returns: cost of shortest path
**/
int best_first_search(int **adj, int v, int *shortest_path);


/**
 *	Uses the Greedy heuristic algorithm  to solves TSP.
 *	Params: adj is adjacency matrix, v is number of nodes
 *	Approxiamte shortest path is stored in shortest_path param
 *	Returns: cost of approximate shortest path
**/
int greedy_heuristic(int **adj, int v, int *shortest_path);

/**
 * 	Tree node to hold path of nodes traversed in graph and cost
 * 	of path. If tree node is leaf, cost equals final cost. If tree
 * 	node is non-leaf, cost equals lower bound on all choices remaining.
**/
class TreeNode
{
public:

	// lower bound, becomes exact cost of path if node is a leaf
	int cost;
	
	// partial path
	int *path;
	
	// length of partial path
	int length;
		
	// Constructor
	TreeNode();
	
	// Destructor
	~TreeNode() { delete[] path; }
};

#endif

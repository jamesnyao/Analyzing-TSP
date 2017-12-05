#ifndef CS375_PROJECT_SRC_TSP_FUNCTIONS_CPP
#define CS375_PROJECT_SRC_TSP_FUNCTIONS_CPP

#include "TSP_Functions.h"
#include <vector>
#include <cstring>

using namespace std;

// Adds all elements in an array (returns sum)
int sum_arr(int *arr, int n);

// Gets next permutation of path
void permute(int *path, int n);

// Gets child paths for branch and bound algorithm
void get_paths(int *path, int total, int fixed, int **paths);

// Gets node with lowest cost from priority queue
TreeNode *get_priority(vector<TreeNode*> &queue);

// Determines if array contains element j
bool contains(int *path, int n, int j);

// Gets lower bound for a TreeNode, given a set of fixed nodes
int bound(int *path, int **adj, int total, int length);

// Gets the minimum index value of an array
int get_min(int* arr, int length);

// Brute force algorithm
int brute_force(int **adj, int v, int *shortest_path)
{
	int i, j, iterations, sum, smallest_sum;
	int *path, *costs;

	// Initializes path as [0, 1, 2, ... , v-1]
	path = new int[v];
	for (i = 0; i < v; i++) path[i] = i;
	
	// Finds number of permutations: iterations = (v-1)!
	for (iterations = 1, i = 2; i < v; iterations *= i++);
	
	// Initialize array of costs
	costs = new int[v];
	
	// For each permutation of path
	for (i = 0, sum = 0, smallest_sum = INT_MAX; i < iterations; i++)
	{
		// Permute path on non-first iteration
		if (i > 0) permute(path, v);
		
		// For each node in path
		for (j = 0; j < v; j++)
		{
			// Use adjcency matrix to get costs
			costs[j] = ((j == v-1) ? adj[path[j]][0] : adj[path[j]][path[j+1]]);
		}
		// Sum all costs
		sum = sum_arr(costs, v);
		
		// If current path is lower cost than lowest-so-far
		if (sum < smallest_sum)
		{
			// Update lowest-so-far sum
			smallest_sum = sum;
			
			// Update lowest-so-far path
			for (j = 0; j < v; j++) shortest_path[j] = path[j];
		}
	}
	
	// Free memory
	delete[] path;
	delete[] costs;
	
	// Returns cost of the lowest-cost path
	return smallest_sum;
}

// Nearest neighbor algorithm
int nearest_neighbor(int **adj, int v, int *shortest_path)
{
	int i, j, k, cost_sum;
	int *costs;
	bool element_found;
	
	// Initializes array of costs
	costs = new int[v];
	
	for (i = 0; i < v; i++) shortest_path[i] = 0;
	
	// Populate shortest path array
	for (i = 1, shortest_path[0] = 0; i < v; i++)
	{
		// Find lowest cost
		for (j = 0, costs[i-1] = INT_MAX; j < v; j++)
		{
			// Avoid duplicate elements in path
			for (k = 0, element_found = false; (k < i) && !element_found; k++)
			{
				if (shortest_path[k] == j) element_found = true;
			}
			if (element_found) continue;
			
			// Find lowest cost
			if (adj[shortest_path[i-1]][j] < costs[i-1])
			{
				costs[i-1] = adj[shortest_path[i-1]][j];
				shortest_path[i] = j;
			}
		}		
	}
	
	// Fill last element in shorest path with last available node
	for (i = 1, element_found = true; (i < v) && element_found; i++)
	{
		// Avoid duplication of elements in left subarray
		for (j = 1, element_found = false; (j < v-1) && !element_found; j++)
		{
			if (i == shortest_path[j]) element_found = true;
		}
		
		// If element not in left subarray, set as last element
		if (!element_found) shortest_path[v-1] = i;
	}
	
	// Set last cost
	costs[v-1] = adj[shortest_path[v-1]][0];
	
	// Add up costs
	cost_sum = sum_arr(costs, v);
	
	// Free memory
	delete[] costs;
	
	// Return total cost
	return cost_sum;
}

// Best first search algorithm
int best_first_search(int **adj, int v, int *shortest_path)
{	
	int i, j, fixed, min_cost, cost;

	min_cost = INT_MAX;
	// Creates priority queue to hold promising TreeNodes
	vector<TreeNode*> queue;
	
	// Creates path variable to hold each TreeNode's path
	int *path;

	// Creates list of paths so parent can return multiple 
	// children's paths
	int **paths;
	
	// Initialize fixed to be number of fixed nodes
	fixed = 1;

	// Initialize first parent TreeNode and push to queue
	TreeNode *node = new TreeNode();

	// Create deep copy of path into TreeNode
	node->path = new int[1];
	node->path[0] = 0;
	
	node->cost = bound(path, adj, v, fixed);
	node->length = 1;
	queue.push_back(node);

	// Until the queue is empty, try to find a better solution
	while (queue.size() != 0)
	{
		// Get the priority node from the queue
		node = get_priority(queue);
		
		// Set fixed to be length of priority node's path
		fixed = node->length;

		// Save the path for creating children paths 
		path = new int[fixed];
		memcpy(path, node->path, fixed * sizeof(int));

		// Save the cost for analyzing min cost
		cost = node->cost;

		// Get rid of priority node
		delete node;

		if (node->cost > min_cost) continue;	

		// Initialize space for each child path (there will be
		// [# total nodes - # fixed nodes] children paths)
		paths = new int*[v - fixed];
		
		// Each child path will have space for # total nodes
		for (i = 0; i < v - fixed; i++)
		{
			paths[i] = new int[fixed + 1];
		}

		// If we have not reached leaf nodes
		if (fixed < v-1)
		{
			// Generate children paths
			get_paths(path, v, fixed, paths);
			
			// For each child path, create a TreeNode and add
			// the child to the queue
			for (i = 0; i < v-fixed; i++)
			{
				TreeNode *child = new TreeNode();
				// Create deep copy of paths[i] into child path
				child->path = new int[fixed + 1];
				for (j = 0; j < fixed + 1; j++)
				{
					child->path[j] = paths[i][j];
				}
				child->cost = bound(child->path, adj, v, fixed + 1);
				child->length = fixed + 1;
				queue.push_back(child);
			}	
		}

		// If priority node is a leaf
		else 
		{
			// If node's cost is better than minimum, update min
			if (cost < min_cost) 
			{
				min_cost = cost;
				// Copy best path into shortest_path
				for (i = 0; i < v-1; i++)
				{
					shortest_path[i] = path[i];
				}
				// Add the last element
				for (i = 0; i < v; i++)
				{
					if (!contains(shortest_path, v-1, i))
					{
						shortest_path[v-1] = i;
						break;
					}	
				}
			}	
		}	
		for (i = 0; i < v - fixed; delete[] paths[i++]);
		delete[] paths;
		delete[] path;
	}
	return min_cost;
}

// Greedy heuristic algorithm
int greedy_heuristic(int **adj, int v, int *shortest_path)
{
	bool found;
	int c, i, j, k, min, loc_i, loc_j, cost;
	int *pairs, *reverse_pairs;
	int **adj_p;
	
	// Initially invalidate all elements in final path array
	for (i = 0; i < v; shortest_path[i++] = -1);
	
	// Copy adj into adj_p array
	adj_p = new int*[v];
	for (i = 0; i < v; adj_p[i++] = new int[v]);
	for (i = 0; i < v; i++) for (j = 0; j < v; j++) adj_p[i][j] = adj[i][j];
	
	// Initialize pairs and reverse_pairs arrays
	pairs = new int[v];
	reverse_pairs = new int[v];
	for (i = 0; i < v; pairs[i] = reverse_pairs[i] = -1, i++);
	
	// Loop until every pair is made
	for (c = 0; c < v-1; c++)
	{
		// Loop through all elements in adjacency matrix
		for (i = 0, min = INT_MAX, loc_i = loc_j = -1; i < v; i++)
		{
			for (j = 0; j < v; j++)
			{
				// Skip diagonal elements in matrix (invalid elements)
				if (i == j) continue;
				
				// Found element in adj smaller than smallest so far
				if ((adj_p[i][j] < min) && (adj_p[i][j] != -1))
				{
					// Set smallest so far (min) and location of element
					min = adj_p[i][j];
					loc_i = i;
					loc_j = j;
				}
			}
		}
		
		// Record pair
		pairs[loc_i] = loc_j;
		reverse_pairs[loc_j] = loc_i;
		
		// Reduce adj_p matrix by removing edges out of loc_i and to loc_j
		for (k = 0; k < v; adj_p[loc_i][k] = adj_p[k][loc_j] = -1, k++);
		
		// Checks if edge connects two connected components
		if ((pairs[loc_j] != -1) && (reverse_pairs[loc_i] != -1))
		{
			// Remove all in and out edges in adj_p
			for (i = 0; i < v; i++) adj_p[i][loc_i] = adj_p[loc_j][i] = -1;
			
			// Connects the two connected components by removing crossing edges between them
			for (i = reverse_pairs[loc_i]; i != -1; i = reverse_pairs[i])
			{
				for (k = loc_j; k != -1; adj_p[k][i] = -1, k = pairs[k]);
			}
			for (j = pairs[loc_j]; j != -1; j = pairs[j])
			{
				for (k = loc_i; k != -1; adj_p[j][k] = -1, k = reverse_pairs[k]);
			}
			
		}
		
		// Case where edge is alone or connects to only one connected component
		else
		{
			// Removes back edges of connected segment from loc_i to prevent cycles
			for (k = loc_j; pairs[k] != -1; adj_p[pairs[k]][loc_i] = -1, k = pairs[k]);
			
			// Removes back edges of connected segment from loc_j to prevent cycles
			for (k = loc_i; k != -1; adj_p[loc_j][k] = -1, k = reverse_pairs[k]);
		}
	}

	// Find unset element
	for (i = 0; reverse_pairs[i] != -1; i++);
	
	// Populate final path
	shortest_path[0] = i;
	for (i = 1; i < v; shortest_path[i] = pairs[shortest_path[i-1]], i++);
	
	// Free memory
	for (i = 0; i < v; delete[] adj_p[i++]);
	delete[] adj_p;
	delete[] pairs;
	delete[] reverse_pairs;
	
	// Add costs
	for (i = cost = 0; i < v; i++)
	{
		// Values are retrieved from original adj matrix (unchanged)
		cost += adj[shortest_path[i]][(i == v-1) ? shortest_path[0] : shortest_path[i+1]];
	}
	
	// Return total cost
	return cost;
}

// Adds all elements in an array (returns sum)
int sum_arr(int *arr, int n)
{
	int sum;
	for (sum = 0, n--; n >=0; sum += arr[n--]);
	return sum;
}
// Gets next permutation of path
void permute(int *path, int n)
{
	int i, j, k, pivot;
	bool element_found;

	// Check each node starting at last to see if it can be incremented
	for (i = n-2, element_found = true; element_found; i--)
	{
		for (j = path[i] + 1; j < n; j++)
		{
			// Traverse left subarray to find element
			for (k = 0, element_found = false; k < i; k++)
			{
				if (element_found = (j == path[k])) break;
			}
			
			// If element not found, update path pivot and move to next element
			if (!element_found)
			{
				path[i] = j;
				pivot = i;
				break;
			}
		}
	}
	
	// Minimize rest of path
	for (i = pivot + 1; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			// Traverse left subarray to find element
			for (k = 0, element_found = false; k < i; k++)
			{
				if (element_found = (j == path[k])) break;
			}
			
			// If element not found, update element and move to next element
			if (!element_found)
			{
				path[i] = j;
				break;
			}
		}
	}
}


// Gets the minimum index value of an array
int get_min(int* arr, int length)
{
	int i;

	// Initialize minimum so far
	int min = INT_MAX;
	
	for (i = 0; i < length; i++)
	{
		// If the value is less than minimum so far, update
		if (arr[i] < min)
		{
			min = arr[i];
		}
	}
	return min;
}

// Gets all children paths for a tree node
void get_paths(int *path, int total, int fixed, int** paths) 
{
	// path = [1] for ex.
	int i, j;

	// Copy path into first <fixed> slots in paths
	for (i = 0; i < total - fixed; i++)
	{
		for (j = 0; j < fixed; j++) 
		{
			paths[i][j] = path[j];
		}
	}
	// Add new each child's path
	for (j = 0, i = 0; j < total; j++)
	{
		// Check if the parent path already contains that node
		if (!contains(path, fixed, j))
		{
			paths[i][fixed] = j;
			i++;
		}
	}
}

// Gets priority TreeNode from queue
TreeNode *get_priority(vector<TreeNode*> &queue) 
{
	
	unsigned int i, n;
	int min, min_ind;
	TreeNode *min_node;

	// Initializes variables to hold min so far and its corresponding index
	min_ind = 0;
	min_node = queue[0];
	min = min_node->cost;

	// Initializes number of elements in queue
	n = queue.size();
	
	// Get the TreeNode instance with the minimum bound
	for (i = 1; i < n; i++)
	{
		// If current node has cost less than min, update it and min_ind
		if (queue[i]->cost < min) 
		{
			min_ind = i;
			min_node = queue[i];
			min = min_node->cost;
		}
	}
	// Overwrite element with min cost with last element
	queue[min_ind] = queue[n-1];
	// Delete last element (second copy)
	queue.pop_back();
	return min_node;
}

// Calculates lower bound on TSP given an array of nodes already chosen
int bound(int *path, int **adj, int total, int length)
{
	int i, j, k, sum, row_ind, col_ind;
	int *possible;
	int	*row;

	// Initializes running bound sum
	sum = 0;

	// Add the cost of going from one fixed node to the next (except for the last one)
	for (i = 0; i < length-1; i++)
	{
		row_ind = path[i];
		col_ind = path[i+1];
		sum += adj[row_ind][col_ind];
	}

	// For every other node, add the minimum cost in the row, but exclude
	// nodes already chosen and self node
	for (i = 0; i < total; i++)
	{
		// Construct list of nodes that could be visited next
		possible = new int[total-length];
		
		// Check if we are looking at the last fixed value
		if (i == path[length-1])
		{
			// For last fixed value: exclude edges to all other fixed nodes
			for (j = k = 0; j < total; j++)
			{
				// Exclude other fixed values (which includes itself)
				if (contains(path, length, j)) continue;
			
				// Otherwise include the value
				row_ind = path[length-1];
				col_ind = j;
				possible[k++] = adj[row_ind][col_ind];

			}
			// Add the minimum value for the last fixed node
			sum += get_min(possible, total-length);
		}
		// Check if we are looking at an unfixed node
		if (!contains(path, length, i))
		{
			// For all unfixed values: exclude edges to non-first fixed nodes
			for (j = k = 0; j < total; j++)
			{
				// Exclude self value
				if (j == i) continue;
				// Exclude other fixed values, unless it is the first
				if (contains(path, length, j) && j != 0) continue;
				// Otherwise include the value
				possible[k++] = adj[i][j];

			}
			sum += get_min(possible, total-length);
		}
	}	
	return sum;
}

bool contains(int *path, int n, int j)
{
	int i;
	for (i = 0; i < n; i++)
	{
		if (path[i] == j)
		{
			return true;
		}
	}
	return false;
}

//Constructor for TreeNode
TreeNode::TreeNode()
{
	cost = 0;
}

#endif

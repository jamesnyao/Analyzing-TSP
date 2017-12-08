#ifndef CS375_PROJECT_SRC_DRIVER_CPP
#define CS375_PROJECT_SRC_DRIVER_CPP

#include "TSP_Functions.h"

#include <iostream>
#include <ctime>
#include <cstdlib>

int main(int argc, char **argv)
{
	int num_nodes, i, j, k, start, end;
	int *shortest_path, *costs;
	int ***adjacencies;
	int runs;
	
	// Number of test matrices tested per algorithm (can be changed)
	runs = 5;
	
	// Check arguments
	if (argc != 2)
	{
		std::cout << "Usage error: \n\t./TSP <num vertices>" << std::endl;
		exit(1);
	}
	
	// Initization
	costs = new int[runs];
	
	// Set num_nodes to argument 1
	num_nodes = atoi(argv[1]);
	
	// Initiallize path array and adjacency matrices
	shortest_path = new int[num_nodes];
	
	// Populates adjacency matrices with random values; there are <runs> many matrices
	adjacencies = new int**[runs];
	for (k = 0; k < runs; k++)
	{
		adjacencies[k] = new int*[num_nodes];
		for (i = 0; i < num_nodes; i++)
		{
			adjacencies[k][i] = new int[num_nodes];
			for (j = 0; j < num_nodes; j++)
			{
				if (i == j) adjacencies[k][i][j] = 0;
				else adjacencies[k][i][j] = rand() % 1000 + 1;
			}
		}
	}
	// Brute force algorithm: prints path and cost
	start = clock();
	for (i = 0; i < runs; i++)
	{
		costs[i] = brute_force(adjacencies[i], num_nodes, shortest_path);
	}
	end = clock();
	std::cout << "Brute force shortest path (" << runs << " runs): " << std::endl;
	for (i = 0; i < runs; std::cout << "\tcost of run " << i << ": " << costs[i++] << std::endl);
	std::cout << "\taverage time per run: " << ((float)end - start)/(CLOCKS_PER_SEC * runs) << " seconds" << std::endl;

	// Best first branch and bound heuristic algorithm: prints path and cost
	start = clock();
	for (i = 0; i < runs; i++)
	{
		costs[i] = best_first_search(adjacencies[i], num_nodes, shortest_path);
	}
	end = clock();
	std::cout << "Best first branch and bound shortest path (" << runs << " runs): " << std::endl;
	for (i = 0; i < runs; std::cout << "\tcost of run " << i << ": " << costs[i++] << std::endl);
	std::cout << "\taverage time per run: " << ((float)end - start)/(CLOCKS_PER_SEC * runs) << " seconds" << std::endl;

	// Greedy heuristic algorithm: prints path and cost
	start = clock();
	for (i = 0; i < runs; i++)
	{
		costs[i] = greedy_heuristic(adjacencies[i], num_nodes, shortest_path);
	}
	end = clock();
	std::cout << "Greedy heuristic shortest path (" << runs << " runs): " << std::endl;
	for (i = 0; i < runs; std::cout << "\tcost of run " << i << ": " << costs[i++] << std::endl);
	std::cout << "\taverage time per run: " << ((float)end - start)/(CLOCKS_PER_SEC * runs) << " seconds" << std::endl;

	// Nearest neighbor algorithm: prints path and cost
	start = clock();
	for (i = 0; i < runs; i++)
	{
		costs[i] = nearest_neighbor(adjacencies[i], num_nodes, shortest_path);
	}
	end = clock();
	std::cout << "Nearest neighbor shortest path (" << runs << " runs): " << std::endl;
	for (i = 0; i < runs; std::cout << "\tcost of run " << i << ": " << costs[i++] << std::endl);
	std::cout << "\taverage time per run: " << ((float)end - start)/(CLOCKS_PER_SEC * runs) << " seconds" << std::endl;
	
	// Free memory
	for (k = 0; k < runs; delete[] adjacencies[k++])
	{
		for (i = 0; i < num_nodes; delete[] adjacencies[k][i++]);
	}
	delete[] adjacencies;
	delete[] shortest_path;
}

#endif

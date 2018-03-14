#pragma once

#include "RiverBranches.h"
#include <vector>

// class of cell
class Cell
{
	RiverNode* center;
	vector<RiverNode*> corners;

public:

	Cell();
	~Cell();
	float getArea();
};


// class of river network
class RiverNetwork
{
public:
	int width, height;
	vector<RiverNode*> nodes;
	vector<RiverBranch*> branches;
	vector<vector<vector<int>>> grid; // tmp
	//this vector stores all the non-terminal nodes that currently can be expanded
	vector<RiverNode*> nonTerminalNodes;
	//current maximum priority
	int maxP;
	//current minimum elevation
	double minElevation;

	//RiverNetwork();
	RiverNetwork(int w = 0, int h = 0);
	~RiverNetwork();

	//create the nodes to start from
	void initialNode();
	//select candidate nodes for expansion
	RiverNode* selectNode(double elevationRange);
	//node expansion operation
	void expandNode(RiverNode* node);
	//validation of the new node
	bool validateNode(RiverNode*);
	//create new node 
	RiverNode* createNode();
};
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

	double e;

	int numW, numH;

	vector<RiverNode*> nodes;
	vector<RiverBranch*> branches;
	vector<vector<RiverBranch*>> grids;
	//this vector stores all the non-terminal nodes that currently can be expanded
	vector<RiverNode*> nonTerminalNodes;
	vector<vector<double>> elevationMap;

	//RiverNetwork();
	RiverNetwork(int w = 0, int h = 0, double e = 0.0);
	~RiverNetwork();

	//create the nodes to start from
	void initialNode();
	//select candidate nodes for expansion
	RiverNode* selectNode(double elevationRange);
	//node expansion operation
	void expandNode(RiverNode* node);
	//get next candidate node
	RiverNode* getCandidate(RiverNode* parent, double angle, int p);
	//validation of the new node
	bool validateNode(RiverNode* node, double boundary, RiverBranch* branch);
	//create new node 
};
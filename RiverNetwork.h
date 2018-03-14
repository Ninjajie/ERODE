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
	//vector<vector<vector<int>>> grid; // tmp

	RiverNetwork();
	RiverNetwork(int w, int h);
	~RiverNetwork();

	//void initialNode();
	//RiverNode* selectNode();
	//void expandNode(RiverNode* node);
	//RiverNode* createNode();
};
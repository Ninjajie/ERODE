#pragma once
#pragma once

#include "RiverBranches.h"
#include <vector>

// class of cell
class Cell
{
public:

	RiverNode* center;
	vector<vec2> corners;

	Cell(RiverNode* center = nullptr);
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
	vector<Cell*> cells;
	vector<vector<RiverBranch*>> grids;
	//this vector stores all the non-terminal nodes that currently can be expanded
	vector<RiverNode*> nonTerminalNodes;
	//the elevation map
	vector<vector<double>> elevationMap;
	vector<vector<double>> elevationMapSmoothed;

	// Maya API
	double ElevationConstraint;
	double ElevationPercentile;
	double DisRatio;
	double ps;
	double pa;
	double RiverWidth;
	double RiverHeight;
	double BranchWidth;
	int NEEDW;

	//current maximum priority
	int maxP;
	//current minimum elevation
	double minElevation;
	double elevationRange;
	//RiverNetwork();
	RiverNetwork(int w = 0, int h = 0, double e = 0.0, double eleC = 5.0,
		double eleP = 0.05, double disR = 0.75, double ps = 0.7,
		double pa = 0.2, double rWid = 1.0, double rH = 10.0,
		double bW = 5.0);
	~RiverNetwork();

	//in case we need to resize, we have to recreate the grid
	void Resize(int newWidth, int newHeight);
	//create the nodes to start from
	void initialNode(bool firstmouth, double firstratio,
		bool secondmouth, double secondratio,
		bool thirdmouth, double thirdratio,
		bool fourthmouth, double fourthratio);
	//select candidate nodes for expansion
	RiverNode* selectNode();
	//node expansion operation
	void expandNode(RiverNode* node);
	//get next candidate node
	RiverNode* getCandidate(RiverNode* parent, double angle, int p);
	//validation of the new node
	bool validateNode(RiverNode* node, double boundary, RiverBranch* branch);
	//this function is account for computing the minimum elevation of current non-terminal nodes
	void refreshMinele();

	//auxiliary function for branching operations
	int SymmetricBranching(RiverNode*);
	int AsymmetricBranching(RiverNode*);
	int Continuation(RiverNode*);


	//read the bitmap file into heightmap arrays
	void readBMP(const std::string file);

	//we dont want to load a bitmap more than once
	void readElevation(const std::string elevationValues);

	//after generating rivers, we write the elevation info into the bitmap
	void writeRivers(const std::string filename, std::string& alteredFile);
	void writeRiversFromElevation(const std::string filename, std::string& carved, std::string carvedSmoothed);

	// carve away river
	void carveRiver();

	// smooth river branch
	double smoothRiver(int px, int py, vector<vector<double>>& map, vector<vector<double>>& kernel);
};
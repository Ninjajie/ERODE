//#include "stdafx.h"
#include "RiverNetwork.h"
#include <random>
#include <algorithm>
#include <cmath>
#include <utility>
#include <fstream>
#include <math.h>
#include <set>

#include <maya/MImage.h>
#include <maya/MString.h>

#include "bitmap_image.hpp"

#define EPSILON 0.0001

#define PIXELX 1024

#define SmoothRadius 9

pair<int, int> mapGrid(vec3 pos, double step) {
	int i = std::floor(pos[0] / step);
	int j = std::floor(pos[1] / step);
	return pair<int, int>(i, j);
}


vector<pair<int, int>> branchIndices(vec3 startP, vec3 endP, int NEEDW, double BranchLength, double BranchWidth, double DisRatio, double step, bool pixelwise, int type) {

	//vec3 startP = branch->start->position;
	//vec3 endP = branch->end->position;

	vector<pair<int, int>> res;

	double deltax = endP[0] - startP[0];
	double deltay = endP[1] - startP[1];
	//if the line segment is near perpendicular
	if (abs(deltax) < EPSILON) {
		//swap(start, end);
		swap(deltax, deltay);
		swap(startP[0], startP[1]);
		swap(endP[0], endP[1]);
		pair<int, int> start = mapGrid(startP, step);
		pair<int, int> end = mapGrid(endP, step);
		if (start.first > end.first) {
			std::swap(startP, endP);
			std::swap(start, end);
		}
		int maxX = NEEDW;
		double singleStep = BranchLength * DisRatio;
		if (pixelwise)
		{
			maxX = PIXELX;
			singleStep = 1.0;
		}
		if (type == 0 || !pixelwise) {
			for (int x = start.first; x <= end.first && x < maxX; ++x) {
				double s0 = max(startP[0], x * singleStep);
				double s1 = min(endP[0], (x + 1) * singleStep);


				double y1 = (s0 - startP[0]) * deltay / deltax + startP[1];
				double y2 = (s1 - startP[0]) * deltay / deltax + startP[1];

				int ys = std::floor(y1 / step);
				int ye = std::floor(y2 / step);

				if (ys > ye) {
					swap(ys, ye);
				}
				for (int y = ys; y <= ye && y < maxX; ++y) {
					res.push_back(pair<int, int>(y, x));
				}
			}
		}
		else if (type == 1) {

			swap(start.first, start.second);
			swap(end.first, end.second);
			vec2 half((start.first + end.first) / 2.0, (start.second + end.second) / 2.0);
			vec2 dir(1.0, 0.0);


			vec2 m1 = (vec2(start.first, start.second) + half) / 2.0;
			vec2 m2 = (vec2(end.first, end.second) + half) / 2.0;

			vec2 l = m1 - dir * BranchWidth;
			vec2 r = m2 + dir * BranchWidth;

			// De Casteljau's algorithm
			set<pair<int, int>> points;

			int dx = floor(sqrt(deltax * deltax + deltay * deltay) / 2.0);

			int num = dx * 20.0;
			double dt = 1.0 / (double)num;

			double t = 0;
			for (int i = 0; i < num; ++i) {
				double x = pow(1 - t, 2.0) * start.first + 2.0 * (1 - t) * t * l[0] + pow(t, 2.0) * half[0];
				double y = pow(1 - t, 2.0) * start.second + 2.0 * (1 - t) * t * l[1] + pow(t, 2.0) * half[1];
				points.insert(pair<int, int>(x, y));
				t += dt;
			}

			t = 0;
			for (int i = 0; i < num; ++i) {
				double x = pow(1 - t, 2.0) * half[0] + 2.0 * (1 - t) * t * r[0] + pow(t, 2.0) * end.first;
				double y = pow(1 - t, 2.0) * half[1] + 2.0 * (1 - t) * t * r[1] + pow(t, 2.0) * end.second;
				points.insert(pair<int, int>(x, y));
				t += dt;
			}

			for (auto s : points) {
				res.push_back(s);
			}
		}

		return res;
	}
	//else, as normal
	pair<int, int> start = mapGrid(startP, step);
	pair<int, int> end = mapGrid(endP, step);

	if (start.first > end.first) {
		std::swap(startP, endP);
		std::swap(start, end);
	}

	int maxX = NEEDW;
	double singleStep = BranchLength * DisRatio;
	if (pixelwise)
	{
		maxX = PIXELX;
		singleStep = 1.0;
	}

	if (type == 0 || !pixelwise) {
		for (int x = start.first; x <= end.first && x < maxX; ++x) {
			double s0 = max(startP[0], x * singleStep);
			double s1 = min(endP[0], (x + 1) * singleStep);

			double y1 = (s0 - startP[0]) * deltay / deltax + startP[1];
			double y2 = (s1 - startP[0]) * deltay / deltax + startP[1];

			int ys = std::floor(y1 / step);
			int ye = std::floor(y2 / step);

			if (ys > ye) {
				swap(ys, ye);
			}
			for (int y = ys; y <= ye; ++y) {
				res.push_back(pair<int, int>(x, y));
			}
		}
	}
	else if (type == 1) {

		vec2 half((start.first + end.first) / 2.0, (start.second + end.second) / 2.0);
		vec2 dir(deltay, -deltax);
		dir.Normalize();

		vec2 m1 = (vec2(start.first, start.second) + half) / 2.0;
		vec2 m2 = (vec2(end.first, end.second) + half) / 2.0;

		vec2 l = m1 - dir * BranchWidth;
		vec2 r = m2 + dir * BranchWidth;

		// De Casteljau's algorithm
		set<pair<int, int>> points;

		int dx = floor(sqrt(deltax * deltax + deltay * deltay) / 2.0);

		int num = dx * 20.0;
		double dt = 1.0 / (double)num;

		double t = 0;
		for (int i = 0; i < num; ++i) {
			double x = pow(1 - t, 2.0) * start.first + 2.0 * (1 - t) * t * l[0] + pow(t, 2.0) * half[0];
			double y = pow(1 - t, 2.0) * start.second + 2.0 * (1 - t) * t * l[1] + pow(t, 2.0) * half[1];
			points.insert(pair<int, int>(x, y));
			t += dt;
		}

		t = 0;
		for (int i = 0; i < num; ++i) {
			double x = pow(1 - t, 2.0) * half[0] + 2.0 * (1 - t) * t * r[0] + pow(t, 2.0) * end.first;
			double y = pow(1 - t, 2.0) * half[1] + 2.0 * (1 - t) * t * r[1] + pow(t, 2.0) * end.second;
			points.insert(pair<int, int>(x, y));
			t += dt;
		}

		for (auto s : points) {
			res.push_back(s);
		}

	}

	return res;
}

vector<pair<int, int>> branchIndices(RiverBranch* branch, int NEEDW, double BranchLength, double BranchWidth, double DisRatio, double step, bool pixelwise) {
	return branchIndices(branch->start->position, branch->end->position, NEEDW, BranchLength, BranchWidth, DisRatio, step, pixelwise, branch->branchType);
}


int clamp(int a, int l, int r) {
	return (a > r ? r : (a < l ? l : a));
}


vector<vector<double>> getKernel(int radius) {

	vector<vector<double>> kernel(radius * 2 + 1, vector<double>(radius * 2 + 1, 0.0));

	double sigma = 1.0;
	double s = 2.0 * sigma * sigma, r;

	double sum = 0.0;

	for (int x = -radius; x <= radius; ++x) {
		for (int y = -radius; y <= radius; ++y) {
			r = sqrt(x * x + y * y);
			kernel[x + radius][y + radius] = (exp(-(r * r) / s)) / (M_PI * s);
			sum += kernel[x + radius][y + radius];
		}
	}

	for (int i = 0; i < radius * 2 + 1; ++i) {
		for (int j = 0; j < radius * 2 + 1; ++j) {
			kernel[i][j] /= sum;
		}
	}

	return kernel;
}



RiverNetwork::RiverNetwork(int w, int h, double e, double eleC, double eleP,
	double disR, double ps, double pa, double rWid, double rH, double bW)
	:width(w), height(h), e(e), minElevation(0.0), ElevationConstraint(eleC),
	ElevationPercentile(eleP), DisRatio(disR), ps(ps), pa(pa), RiverWidth(rWid),
	RiverHeight(rH), BranchWidth(bW)
{
	NEEDW = ceil(PIXELX / (e * DisRatio));
	numW = std::ceil((double)width / (DisRatio * e));
	numH = std::ceil((double)height / (DisRatio * e));
	grids.resize(numH * numW, vector<RiverBranch*>());
	elevationRange = 25.0;
}

RiverNetwork::~RiverNetwork()
{
	//delete all the nodes
	for (int i = 0; i < nodes.size(); i++)
	{
		delete nodes[i];
	}
}

//after we import an image, we may need to resize the river map and therefore the underlying grids
void RiverNetwork::Resize(int newWidth, int newHeight)
{
	this->width = newWidth;
	this->height = newHeight;
	numW = std::ceil((double)width / (DisRatio * e));
	numH = std::ceil((double)height / (DisRatio * e));
	grids.resize(numH * numW, vector<RiverBranch*>());
}

//select the nodes to start from
//here is the simplified version, where we choose a random position on each edge
//the initial nodes all have priority of 1, which is the lowest priority
void RiverNetwork::initialNode(bool firstmouth, double firstratio,
	bool secondmouth, double secondratio,
	bool thirdmouth, double thirdratio,
	bool fourthmouth, double fourthratio)
{
	//create random real numbers between [0,1]
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.0, 1.0);
	//multiply those random numbers by actual dimensions to get random positions on edges
	double ratio1 = dis(gen);
	double l1 = width * ratio1;
	double ratio2 = dis(gen);
	double l2 = height * ratio2;
	double ratio3 = dis(gen);
	double l3 = width * ratio3;
	double ratio4 = dis(gen);
	double l4 = height * ratio4;

	//randomly determine the starting priorities
	int p[4];
	for (int i = 0; i < 4; i++)
	{
		double neta = dis(gen);
		if (neta >= 0.7) {
			p[i] = 10;
		}
		else {
			p[i] = 11;
		}
	}
	//declare the index 
	vector<pair<int, int>> idx;
	//create 4 mouths around the boundary
	if (firstmouth)
	{
		RiverNode* mouth1 = new RiverNode();
		if (firstratio >= 0.0 && firstratio <= 1.0)
		{
			l1 = width * firstratio;
		}

		//RiverNode* mouth1 = new RiverNode(p[0], vec3(l1, 0, 0), nullptr);
		mouth1->priority = p[0];
		mouth1->position = vec3(l1, 0, 0);
		mouth1->parent = nullptr;
		nodes.push_back(mouth1);

		RiverNode* mouth11 = new RiverNode(p[0], vec3(l1, e, 0), mouth1);
		mouth11->id = mouth1->id;
		//mouth11->setElevation(elevationMap[(int)(l1/ (DisRatio * e))][(int)(e/ (DisRatio * e))]);
		mouth11->position[2] = mouth11->getElevation(height, width, this->elevationMap);
		nodes.push_back(mouth11);
		nonTerminalNodes.push_back(mouth11);
		RiverBranch* branch1 = new RiverBranch(mouth1, mouth11);
		branches.push_back(branch1);
		idx = branchIndices(branch1, NEEDW, e, BranchWidth, DisRatio, e * DisRatio, false);
		for (auto id : idx) {
			grids[id.first * numW + id.second].push_back(branch1);
		}

	}
	if (secondmouth)
	{
		RiverNode* mouth2 = new RiverNode();
		if (secondratio >= 0.0 && secondratio <= 1.0)
		{
			l2 = height* secondratio;
		}

		//RiverNode* mouth1 = new RiverNode(p[0], vec3(l1, 0, 0), nullptr);
		mouth2->priority = p[1];
		mouth2->position = vec3((double)width, l2, 0);
		mouth2->parent = nullptr;
		nodes.push_back(mouth2);

		RiverNode* mouth22 = new RiverNode(p[1], vec3((double)width - e, l2, 0), mouth2);
		mouth22->id = mouth2->id;
		//mouth22->setElevation(elevationMap[(int)(((double)width - e) / (DisRatio * e))][(int)(l2 / (DisRatio * e))]);
		mouth22->position[2] = mouth22->getElevation(height, width, this->elevationMap);
		nodes.push_back(mouth22);
		nonTerminalNodes.push_back(mouth22);
		RiverBranch* branch2 = new RiverBranch(mouth2, mouth22);
		branches.push_back(branch2);
		idx = branchIndices(branch2, NEEDW, e, BranchWidth, DisRatio, e * DisRatio, false);
		for (auto id : idx) {
			grids[id.first * numW + id.second].push_back(branch2);
		}

	}

	if (thirdmouth)
	{
		RiverNode* mouth3 = new RiverNode();
		if (thirdratio >= 0.0 && thirdratio <= 1.0)
		{
			l3 = width * thirdratio;
		}

		//RiverNode* mouth1 = new RiverNode(p[0], vec3(l1, 0, 0), nullptr);
		mouth3->priority = p[2];
		mouth3->position = vec3(l3, (double)height, 0);
		mouth3->parent = nullptr;
		nodes.push_back(mouth3);

		RiverNode* mouth33 = new RiverNode(p[2], vec3(l3, (double)height - e, 0), mouth3);
		mouth33->id = mouth3->id;
		//mouth33->setElevation(elevationMap[(int)(l3 / (DisRatio * e))][(int)(((double)height - e) / (DisRatio * e))]);
		mouth33->position[2] = mouth33->getElevation(height, width, this->elevationMap);
		nodes.push_back(mouth33);
		nonTerminalNodes.push_back(mouth33);
		RiverBranch* branch3 = new RiverBranch(mouth3, mouth33);
		branches.push_back(branch3);
		idx = branchIndices(branch3, NEEDW, e, BranchWidth, DisRatio, e * DisRatio, false);
		for (auto id : idx) {
			grids[id.first * numW + id.second].push_back(branch3);
		}

	}

	if (fourthmouth)
	{
		RiverNode* mouth4 = new RiverNode();
		if (fourthratio >= 0.0 && fourthratio <= 1.0)
		{
			l4 = height * fourthratio;
		}

		//RiverNode* mouth1 = new RiverNode(p[0], vec3(l1, 0, 0), nullptr);
		mouth4->priority = p[3];
		mouth4->position = vec3(0, l4, 0);
		mouth4->parent = nullptr;
		nodes.push_back(mouth4);


		RiverNode* mouth44 = new RiverNode(p[3], vec3(e, l4, 0), mouth4);
		mouth44->id = mouth4->id;
		//mouth44->setElevation(elevationMap[(int)(e / (DisRatio * e))][(int)(l4 / (DisRatio * e))]);
		mouth44->position[2] = mouth44->getElevation(height, width, this->elevationMap);
		nodes.push_back(mouth44);
		nonTerminalNodes.push_back(mouth44);
		RiverBranch* branch4 = new RiverBranch(mouth4, mouth44);
		branches.push_back(branch4);
		idx = branchIndices(branch4, NEEDW, e, BranchWidth, DisRatio, e * DisRatio, false);
		for (auto id : idx) {
			grids[id.first * numW + id.second].push_back(branch4);
		}

	}

	//we first apply a continuation for the initial mouths

	//initialize the minimum elevation
	if (nonTerminalNodes.size() > 0)
	{
		minElevation = nonTerminalNodes[0]->position[2];
		for (int i = 0; i < nonTerminalNodes.size(); i++)
		{
			if (nonTerminalNodes[i]->position[2] <= minElevation)
				minElevation = nonTerminalNodes[i]->position[2];
		}
	}
	else
	{
		minElevation = -1.0;
	}
}


void RiverNetwork::refreshMinele()
{
	double tempEle = 10000;
	for (int i = 0; i < nonTerminalNodes.size(); ++i)
	{
		if (nonTerminalNodes[i]->position[2] < tempEle)
			tempEle = nonTerminalNodes[i]->position[2];
	}
	minElevation = tempEle;
	//std::cout << "minEle" << minElevation << std::endl;
}
//from all the non-terminal nodes, select exactly one node that is subject to expansion
//based on the elevationRange and priorities
RiverNode * RiverNetwork::selectNode()
{

	//loop in all the non-terminal nodes, find the one that has the highest priority that lies within the 
	//elevation range of [z, z+elevationRange]

	if (nonTerminalNodes.size() == 0)
	{
		return nullptr;
	}

	//first need to re-compute the min elevation of current non-terminal nodes
	this->refreshMinele();
	//cadidateNodes is the set of all nodes with z,z+eR
	vector<RiverNode*> candidateNodes;
	//find the nodes within [z, z+elevationRange]
	for (int i = 0; i < nonTerminalNodes.size(); i++)
	{
		if (nonTerminalNodes[i]->position[2] <= minElevation + elevationRange)
			candidateNodes.push_back(nonTerminalNodes[i]);
	}
	//find the highest priority value in candidateNodes
	//if (candidateNodes.size() == 0) {
	//	elevationRange
	//}
	int maxP = candidateNodes[0]->priority;
	for (int i = 0; i < candidateNodes.size(); i++)
	{
		if (candidateNodes[i]->priority >= maxP) maxP = candidateNodes[i]->priority;
	}
	vector<RiverNode*> finalcandidateNodes;
	// if all the nonterminal is of priority 1, we increase all their priorities by 1
	if (maxP == 1)
	{
		maxP = 2;
		for (int i = 0; i < candidateNodes.size(); i++)
		{
			candidateNodes[i]->priority = maxP;
			finalcandidateNodes.push_back(candidateNodes[i]);
		}
	}
	else
	{
		//find the set of the nodes with the highest priority value
		for (int i = 0; i < candidateNodes.size(); i++)
		{
			if (candidateNodes[i]->priority == maxP)
				finalcandidateNodes.push_back(candidateNodes[i]);
		}
	}
	//if this set has more than one element, select the one with lowest elevation
	if (finalcandidateNodes.size() > 1)
	{

		int finalIndex = 0;
		double minele = finalcandidateNodes[0]->position[2];
		for (int i = 0; i < finalcandidateNodes.size(); i++)
		{
			if (finalcandidateNodes[i]->position[2] <= minele) {
				finalIndex = i;
				minele = finalcandidateNodes[i]->position[2];
			}
		}
		return finalcandidateNodes[finalIndex];
	}
	else {
		return finalcandidateNodes[0];
	}

}

//given the selected candidate node, choose from three different situations
//(symmetric/asymmetric/continuation)
//and compute new branches from them
void RiverNetwork::expandNode(RiverNode * node)
{
	//if priority >1 we have three different situations
	if (node->priority > 1)
	{
		double prob = (double)std::rand() / (double)RAND_MAX;
		// symmetric
		if (prob >= 0.0 && prob < ps) {
			this->SymmetricBranching(node);
		}
		// asymmetric
		else if (prob > ps && prob <= (ps + pa)) {
			this->AsymmetricBranching(node);
		}
		// continuation
		else {
			this->Continuation(node);
		}
	}
	//else only continuation
	else {
		this->Continuation(node);
	}
	// After expansion part, we need to set this node to terminal(remove it from nonTerminals
	int ind = 0;
	for (int i = 0; i < nonTerminalNodes.size(); i++)
	{
		if (nonTerminalNodes[i] == node) {
			ind = i;
			break;
		}
	}
	nonTerminalNodes.erase(nonTerminalNodes.begin() + ind);
	//std::cout << nonTerminalNodes.size() << std::endl;

}

RiverNode* RiverNetwork::getCandidate(RiverNode* node, double angle, int p) {
	vec2 ppos = vec2(node->parent->position[0], node->parent->position[1]);
	vec2 pos = vec2(node->position[0], node->position[1]);
	vec2 dir = pos - ppos;
	dir = dir.Normalize();
	vec2 perpen = vec2(dir[1], -dir[0]);
	vec2 FinalDir = dir * std::sin(Deg2Rad * angle) + perpen * std::cos(Deg2Rad * angle);
	//double dx = e * std::cos(Deg2Rad * angle), dy = e * std::sin(Deg2Rad * angle);
	RiverNode* resultNode = new RiverNode(p, vec3(pos[0] + e * FinalDir[0], pos[1] + e * FinalDir[1], node->position[2] /*+ elevation*/), node);
	resultNode->setElevation(resultNode->getElevation(height, width, this->elevationMap));
	return resultNode;
}


//In this function, the input is a possible new node to be added to the network
//this function check if this node is far enough from:
//1.the boundary of terrain and 
//2.all other branches


bool RiverNetwork::validateNode(RiverNode * node, double boundary, RiverBranch* branch)
{

	// check elevation correctness
	if (node->position[2] < node->parent->position[2] * (1.0 - ElevationPercentile)) {
		return false;
	}
	// check if within boundary
	if (node->position[0] < boundary ||
		node->position[0] > (double)width - boundary ||
		node->position[1] < boundary ||
		node->position[1] > (double)height - boundary) {
		return false;
	}
	// check collision with other branches
	branch = new RiverBranch(node->parent, node);
	vector<pair<int, int>> indices = branchIndices(branch, NEEDW, e, BranchWidth, DisRatio, e * DisRatio, false);
	pair<int, int> parentIdx = mapGrid(node->parent->position, e * DisRatio);
	for (auto id : indices) {
		//skip the parent grid
		if (id.first == parentIdx.first && id.second == parentIdx.second) continue;
		//for all the left indices, check if there are any branches inside
		int idx = id.first * numW + id.second;
		if (id.first >= 0 && id.first < numW &&
			id.second >= 0 && id.second < numH)
		{
			//if there is a branch inside, then just return false
			if (grids[idx].size() != 0)
			{
				branch = nullptr;
				return false;
			}
			//else we need to check all the surronding grids and compute the distance
			else
			{
				for (int verInd = -1; verInd <= 1; verInd++)
				{
					for (int horInd = -1; horInd <= 1; horInd++)
					{
						int subidx = (id.first + horInd) * numW + (id.second + verInd);
						if (id.first + horInd >= 0 && id.first + horInd < numW &&
							id.second + verInd >= 0 && id.second + verInd< numH)
						{
							//if there is at least one branch inside, then compute the distance
							if (grids[subidx].size() != 0)
							{
								//iterate the branches inside, compute the distatnce
								for (int indBranches = 0; indBranches < grids[subidx].size(); indBranches++)
								{
									RiverBranch* existBranch = grids[subidx][indBranches];
									//we need to check if this branch is his parent or brother/sister
									if (existBranch->start == branch->start || existBranch->end == branch->start)continue;
									double distance = branch->distance(existBranch);
									if (distance < DisRatio * e)
									{
										branch = nullptr;
										return false;
									}
								}
							}
						}
						//else out of range
						else continue;
					}
				}
			}
		}
		//our of range
		else {
			branch = nullptr;
			return false;
		}
	}
	//no problem, add this branch to branch list
	branches.push_back(branch);
	//tell grid that this branch is inside
	for (auto id : indices) {
		grids[id.first * numW + id.second].push_back(branch);
	}

	return true;

}

//auxiliary functions for branching
int RiverNetwork::SymmetricBranching(RiverNode* node)
{
	//check the final # of added nodes
	int nodesAdded = 0;
	//loop count
	int num = 2;
	while (num) {
		double initialAngle = 45.0 + 90.0 * (num % 2), currentAngle = initialAngle;
		double angleStep = 2.5;
		RiverNode* newNode = nullptr;
		RiverBranch* branch = nullptr;
		//currentAngle >= -2.5 && <= 182.5 means that it can surpass the bottom line of 0/180 degree for once.
		for (int k = 0; currentAngle >= 0 - angleStep && currentAngle <= 180 + angleStep; k++)
		{
			currentAngle = currentAngle + pow(-1.0, k) * (k / 2) * angleStep;
			int newP = node->priority - 1;
			if (newP < 1)newP = 1;
			newNode = getCandidate(node, currentAngle, newP);
			//if a new node is avaliable at some position, add this node to the node list 
			//also add this node to its parent's children list
			if (validateNode(newNode, e * DisRatio, branch))
			{
				newNode->id = node->id;
				node->children.push_back(newNode);
				this->nodes.push_back(newNode);
				//add this newNode to nonterminal 
				this->nonTerminalNodes.push_back(newNode);
				break;
			}
		}
		num--;
	}
	return nodesAdded;
}
int RiverNetwork::AsymmetricBranching(RiverNode* node)
{
	//check the final # of added nodes
	int nodesAdded = 0;
	//loop count
	int num = 2;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.7, 0.99);
	double ratio = dis(gen);
	int newP = (int)(ratio * node->priority);
	if (newP < 1)newP = 1;
	//set a random priority for the second node
	int p[2] = { node->priority, newP };
	while (num) {
		//int k = 0;
		double initialAngle = 45.0 + 90.0 * (num % 2), currentAngle = initialAngle;
		double angleStep = 2.5;
		RiverNode* newNode = nullptr;
		RiverBranch* branch = nullptr;
		//currentAngle >= -2.5 && <= 182.5 means that it can surpass the bottom line of 0/180 degree for once.
		for (int k = 0; currentAngle >= 0 - angleStep && currentAngle <= 180 + angleStep; k++)
		{
			currentAngle = currentAngle + pow(-1.0, k) * (k / 2) * angleStep;
			newNode = getCandidate(node, currentAngle, p[num - 1]);
			//if a new node is avaliable at some position, add this node to the node list 
			//also add this node to its parent's children list
			//also add the branch to the branch list
			if (validateNode(newNode, e * 0.25, branch))
			{
				newNode->id = node->id;
				node->children.push_back(newNode);
				this->nodes.push_back(newNode);
				//add this to nonterminal 
				this->nonTerminalNodes.push_back(newNode);
				nodesAdded++;
				break;
			}
		}
		num--;
	}
	return nodesAdded;
}
int RiverNetwork::Continuation(RiverNode* node)
{
	int k = 0;
	double initialAngle = 90.0, currentAngle = initialAngle;
	double angleStep = 2.5;
	RiverNode* newNode = nullptr;
	RiverBranch* branch = nullptr;
	//currentAngle >= -2.5 && <= 182.5 means that it can surpass the bottom line of 0/180 degree for once.
	for (int k = 0; currentAngle >= 0 - angleStep && currentAngle <= 180 + angleStep; k++)
	{
		currentAngle = currentAngle + pow(-1, k) * (k / 2) * angleStep;
		newNode = getCandidate(node, currentAngle, node->priority);

		//if a new node is avaliable at some position, add this node to the node list 
		//also add this node to its parent's children list
		if (validateNode(newNode, e * 0.25, branch))
		{
			newNode->id = node->id;
			node->children.push_back(newNode);
			this->nodes.push_back(newNode);
			//add this to nonterminal 
			this->nonTerminalNodes.push_back(newNode);
			return 1;
		}
	}
	return 0;
}



void RiverNetwork::readBMP(const std::string file)
{
	MImage rawimage;
	rawimage.readFromFile(MString(file.c_str()));

	

	unsigned int width, height;
	rawimage.getSize(width, height);
	unsigned int imagesize = width * height;

	unsigned char *pixels = rawimage.pixels();
	unsigned int i;
	unsigned int j;
	ofstream heightValues("C:/Test/Testheightvalues.txt");
	for (i = 0; i < height; i++)
	{
		std::vector<double> oneRow;
		for (j = 0; j < width; j++)
		{
			heightValues << static_cast<int>(*(pixels));
			oneRow.push_back(static_cast<double>(*(pixels)));
			pixels += 4;
		}
		this->elevationMap.push_back(oneRow);
	}
	//for (i = 0; i < imagesize; i++)
	//{
	//	if (i <= imagesize / 2) {
	//		*(pixels++) = (unsigned char)(0);
	//		*(pixels++) = (unsigned char)(0);
	//		*(pixels++) = (unsigned char)(0);
	//		pixels++;
	//	}
	//}

	//bitmap_image image(file);

	//if (!image)
	//{
	//	printf("Error - Failed to open: input.bmp\n");
	//	return;
	//}

	//unsigned int total_number_of_pixels = 0;

	//const unsigned int height = image.height();
	//const unsigned int width = image.width();



	//ofstream heightValues("heightvalues.txt");
	// get the vector <R,G,B> for the pixel at (w,h)
	//for (std::size_t y = 0; y < height; ++y)
	//{
	//	std::vector<double> oneRow;
	//	for (std::size_t x = 0; x < width; ++x)
	//	{
	//		// get the vector <R,G,B> for the pixel at (1,1)
	//		rgb_t colour;
	//		image.get_pixel(x, y, colour);
	//		heightValues << static_cast<int>(colour.red) << std::endl;
	//		oneRow.push_back(static_cast<double>(colour.red));
	//	}
	//	this->elevationMap.push_back(oneRow);
	//}
}

void RiverNetwork::readElevation(const std::string elevationValues)
{
	ifstream elevations(elevationValues, std::ios::in);
	for (int i = 0; i < 1024; i++)
	{
		std::vector<double> oneRow;
		for (int j = 0; j < 1024; j++)
		{
			double toBeInput = 0.0;
			elevations >> toBeInput;
			oneRow.push_back(toBeInput);
		}
		this->elevationMap.push_back(oneRow);
	}
}

void RiverNetwork::writeRivers(const std::string filename, std::string& alteredFile)
{
	MImage rawimage;
	rawimage.readFromFile(MString(filename.c_str()));
	unsigned int width, height;
	rawimage.getSize(width, height);
	MImage newimage;
	newimage.create(width, height);
	unsigned char *newpixels = newimage.pixels();

	unsigned int imagesize = width * height;

	unsigned char *pixels = rawimage.pixels();
	unsigned int i;
	unsigned int j;
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			*(newpixels++) = *pixels;
			*(newpixels++) = *pixels;
			*(newpixels++) = *pixels;
			newpixels++;
			pixels += 4;
			
		}
	}
	
	//then for all the branches, we alter the corresponding pixel values
	for (auto branch : this->branches)
	{
		//here for the only time we pass true to the pixelwise argument

		std::vector<pair<int, int>> corespondPixels = branchIndices(branch, NEEDW, e, BranchWidth, DisRatio, 1.0, true);
		for (auto indices : corespondPixels)
		{

			//return newpixels to the first pixel!!!!!!!!!!
			newpixels = newimage.pixels();
			unsigned int offset = indices.first * width + indices.second ;
			*(newpixels + offset * 4) = (unsigned char)(0);
			*(newpixels + offset * 4 + 1) = (unsigned char)(0);
			*(newpixels + offset * 4 + 2) = (unsigned char)(0);
		}
	}

    alteredFile = filename;
    
    std::size_t pos = alteredFile.size() - (std::size_t)(4);
    
	alteredFile = alteredFile.substr(0, pos);
    
	alteredFile += "Altered.jpg";

	newimage.writeToFile(MString(alteredFile.c_str()), "jpg");
	return;

}


void RiverNetwork::writeRiversFromElevation(const std::string filename, std::string& carved, std::string carvedSmoothed) {

	MImage rawimage;
	rawimage.readFromFile(MString(filename.c_str()));
	unsigned int width, height;
	rawimage.getSize(width, height);

	MImage newimage;
	newimage.create(width, height);
	unsigned char *newpixels = newimage.pixels();

	unsigned int i;
	unsigned int j;
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			*(newpixels++) = (unsigned char)(elevationMapSmoothed[i][j]);
			*(newpixels++) = (unsigned char)(elevationMapSmoothed[i][j]);
			*(newpixels++) = (unsigned char)(elevationMapSmoothed[i][j]);
			newpixels++;
		}
	}
	carved = filename;

	std::size_t pos = carved.size() - (std::size_t)(4);

	carved = carved.substr(0, pos);

	carved += "Carved.jpg";

	newimage.writeToFile(MString(carved.c_str()), "jpg");


	//MImage newimage2;
	//newimage2.create(width, height);
	//unsigned char *newpixels2 = newimage2.pixels();

	//unsigned int i;
	//unsigned int j;
	//for (i = 0; i < height; i++)
	//{
	//	for (j = 0; j < width; j++)
	//	{
	//		*(newpixels2++) = (unsigned char)(elevationMapSmoothed[i][j]);
	//		*(newpixels2++) = (unsigned char)(elevationMapSmoothed[i][j]);
	//		*(newpixels2++) = (unsigned char)(elevationMapSmoothed[i][j]);
	//		newpixels2++;
	//	}
	//}
	//carvedSmoothed = filename;

	//std::size_t pos = carvedSmoothed.size() - (std::size_t)(4);

	//carvedSmoothed = carvedSmoothed.substr(0, pos);

	//carvedSmoothed += "CarvedSmoothed.jpg";

	//newimage2.writeToFile(MString(carvedSmoothed.c_str()), "jpg");
	return;
}

// carve away river
void RiverNetwork::carveRiver() {

	vector<vector<double>> tmpMap = elevationMap;

	// for each branch, find rightmost point and leftmost point for every pixel along its carving direction 
	for (auto branch : branches) {

		vec2 dir;
		dir = branch->getCarveDirection();

		std::vector<pair<int, int>> pixels = branchIndices(branch, NEEDW, e, BranchWidth, DisRatio, 1.0, true);

		double startW = log(branch->start->priority * 5.0), endW = log(branch->end->priority * 5.0);

		for (auto p : pixels) {

			vec2 origin(p.first, p.second);

			double t = std::sqrt(std::pow(origin[0] - branch->start->position[0], 2.0) +
				std::pow(origin[1] - branch->start->position[1], 2.0)) /
				std::sqrt(std::pow(branch->end->position[0] - branch->start->position[0], 2.0) +
					std::pow(branch->end->position[1] - branch->start->position[1], 2.0));

			t = startW + (endW - startW) * t;

			vec2 l = -RiverWidth * t * dir + origin;
			vec2 r = RiverWidth * t * dir + origin;

			pair<int, int> left(l[0], l[1]), right(r[0], r[1]);

			// make sure the left point is left to origin point w.r.t world space
			if (left.first > right.first) {
				std::swap(left, right);
			}

			left.first = clamp(left.first, 0, tmpMap.size() - 1);
			left.second = clamp(left.second, 0, tmpMap.size() - 1);
			right.first = clamp(right.first, 0, tmpMap.size() - 1);
			right.second = clamp(right.second, 0, tmpMap.size() - 1);


			double heightL = tmpMap[left.first][left.second];
			double heightR = tmpMap[right.first][right.second];

			// calculate the height of middle point
			double lowestHeight = std::min(heightL, heightR) - RiverHeight * t;
			lowestHeight = lowestHeight > 0.0 ? lowestHeight : 0.0;

			auto pointsAlongDir = branchIndices(vec3(left.first, left.second, 0), vec3(right.first, right.second, 0), NEEDW, e, BranchWidth, DisRatio, 1.0, true, 0);

			double dh1 = heightL - lowestHeight, dh2 = heightR - lowestHeight;

			// hard carving
			for (auto pD : pointsAlongDir) {
				//std::cout << pD.first << " " << pD.second << std::endl;
				if (pD.first < 0 || pD.first >= PIXELX || pD.second < 0 || pD.second >= PIXELX) continue;

				double distance = std::sqrt((pD.first - p.first) * (pD.first - p.first) + (pD.second - p.second) * (pD.second - p.second));

				int isLeft = pD.first < p.first ? 1 : 0;

				double dh = isLeft * dh1 + (1 - isLeft) * dh2;

				double curHeight = dh * distance / (RiverWidth * t) + lowestHeight;
				elevationMap[pD.first][pD.second] = std::min(elevationMap[pD.first][pD.second], curHeight);
			}

		}

	}

	// smooth river

	auto kernel = getKernel(SmoothRadius / 2);

	auto tmpHardMap = elevationMap;
	elevationMapSmoothed = elevationMap;


	for (auto branch : branches) {

		vec2 dir = branch->getCarveDirection();

		vector<pair<int, int>> pixels = branchIndices(branch, NEEDW, e, BranchWidth, DisRatio, 1.0, true);

		double startW = log(branch->start->priority * 5.0), endW = log(branch->end->priority * 5.0);

		for (auto p : pixels) {

			vec2 origin(p.first, p.second);

			double t = std::sqrt(std::pow(origin[0] - branch->start->position[0], 2.0) +
				std::pow(origin[1] - branch->start->position[1], 2.0)) /
				std::sqrt(std::pow(branch->end->position[0] - branch->start->position[0], 2.0) +
					std::pow(branch->end->position[1] - branch->start->position[1], 2.0));

			t = startW + (endW - startW) * t + 0.5;

			vec2 l = -RiverWidth * t * dir + origin;
			vec2 r = RiverWidth * t * dir + origin;

			pair<int, int> left(l[0], l[1]), right(r[0], r[1]);

			// make sure the left point is left to origin point w.r.t world space
			if (left.first > right.first) {
				std::swap(left, right);
			}

			left.first = clamp(left.first, 0, tmpMap.size() - 1);
			left.second = clamp(left.second, 0, tmpMap.size() - 1);
			right.first = clamp(right.first, 0, tmpMap.size() - 1);
			right.second = clamp(right.second, 0, tmpMap.size() - 1);

			auto pointsAlongDir = branchIndices(vec3(left.first, left.second, 0), vec3(right.first, right.second, 0), NEEDW, e, BranchWidth, DisRatio, 1.0, true, 0);

			for (auto pD : pointsAlongDir) {
				elevationMapSmoothed[pD.first][pD.second] = smoothRiver(pD.first, pD.second, tmpHardMap, kernel);
			}
		}

	}

}

double RiverNetwork::smoothRiver(int px, int py, vector<vector<double>>& map, vector<vector<double>>& kernel) {

	double height = 0.0;

	for (int i = 0; i < SmoothRadius; ++i) {
		for (int j = 0; j < SmoothRadius; ++j) {
			int x = px - SmoothRadius / 2 + i;
			int y = py - SmoothRadius / 2 + j;
			if (x < 0) x = -x;
			if (y < 0) y = -y;
			if (x >= PIXELX) x = PIXELX - 1 - (x - PIXELX);
			if (y >= PIXELX) y = PIXELX - 1 - (y - PIXELX);
			height += kernel[i][j] * map[x][y];
		}
	}


	return height;
}


//Voronoi cell class

//Cell constructor
Cell::Cell(RiverNode* center)
	:center(center)
{
	//Todo
}

//Cell destructor
Cell::~Cell()
{
	//Todo
}

//get the area of current cell
float Cell::getArea()
{
	//We use Shoelace formula to compute the area of a polygon
	double area = 0.0;
	int numofCorners = this->corners.size();
	int j = numofCorners - 1;
	for (int i = 0; i < numofCorners; i++)
	{
		area += (this->corners[j][0] + this->corners[i][0]) * (this->corners[j][1] - this->corners[i][1]);
		j = i;
	}
	return abs(area / 2.0);
}

#include "RiverNetwork.h"
#include <random>

RiverNetwork::RiverNetwork(int w = 0, int h = 0)
	:width(w), height(h)
{

}
RiverNetwork::~RiverNetwork()
{

}

//select the nodes to start from
//here is the simplified version, where we choose a random position on each edge
//the initial nodes all have priority of 1, which is the lowest priority
void RiverNetwork::initialNode()
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
			p[i] = 5;
		}
		else {
			p[i] = 4;
		}
	}
	//create 4 mouths around the boundary
	RiverNode* mouth1 = new RiverNode(p[0], vec3(l1, 0, 0), nullptr);
	nodes.push_back(mouth1);
	nonTerminalNodes.push_back(mouth1);
	RiverNode* mouth2 = new RiverNode(p[1], vec3((double)width, l2, 0), nullptr);
	nodes.push_back(mouth2);
	nonTerminalNodes.push_back(mouth2);
	RiverNode* mouth3 = new RiverNode(p[2], vec3(l3, (double)height, 0), nullptr);
	nodes.push_back(mouth3);
	nonTerminalNodes.push_back(mouth3);
	RiverNode* mouth4 = new RiverNode(p[3], vec3(0, l4, 0), nullptr);
	nodes.push_back(mouth4);
	nonTerminalNodes.push_back(mouth4);
}

//from all the non-terminal nodes, select exactly one node that is subject to expansion
//based on the elevationRange and priorities
RiverNode * RiverNetwork::selectNode(double elevationRange)
{
	//Todo
	//loop in all the non-terminal nodes, find the one that has the highest priority that lies within the 
	//elevation range of [z, z+elevationRange]
	if (nonTerminalNodes.size() == 0)
	{
		return nullptr;
	}
	
	vector<RiverNode*> candidateNodes;
	//find the nodes within [z, z+elevationRange]
	for (int i = 0; i < nonTerminalNodes.size(); i++)
	{
		if (nonTerminalNodes[i]->position[2] <= minElevation + elevationRange)
			candidateNodes.push_back(nonTerminalNodes[i]);
	}
	//find the highest priority value in candidateNodes
	int maxP = candidateNodes[0]->priority;
	for (int i = 0; i < candidateNodes.size(); i++)
	{
		if (candidateNodes[i]->priority >= maxP) maxP = candidateNodes[i]->priority;
	}
	//find the set of the nodes with the highest priority value
	vector<RiverNode*> finalcandidateNodes;
	for (int i = 0; i < candidateNodes.size(); i++)
	{
		if (candidateNodes[i]->priority == maxP)
			finalcandidateNodes.push_back(candidateNodes[i]);
	}
	//if this set has more than one element, randomly select one for the final candidate node
	if (finalcandidateNodes.size() > 1)
	{
		std::random_device rdInt;
		std::mt19937 gen(rdInt());
		std::uniform_int_distribution<> dis(0, finalcandidateNodes.size() - 1);
		//get the final selection
		int finalIndex = dis(gen);
		//return the final node selected
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
	//Todo
}

//In this function, the input is a possible new node to be added to the network
//this function check if this node is far enough from:
//1.the boundary of terrain and 
//2.all other branches
bool RiverNetwork::validateNode(RiverNode * node)
{
	//Todo
	return false;
}

//????
RiverNode * RiverNetwork::createNode()
{
	//Todo??
	return nullptr;
}


//Voronoi cell class

//Cell constructor
Cell::Cell()
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
	//Todo
	return 0.0f;
}

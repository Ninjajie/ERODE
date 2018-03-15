#include "RiverBranches.h"

int RiverBranch::id = 0;

RiverNode::RiverNode()
	:priority(1), position(vec3(0,0,0)),parent(nullptr),terminal(false)
{
}


RiverNode::RiverNode(int p, vec3 pos,RiverNode* parent)
	:priority(p), position(pos), parent(parent),terminal(false)
{
}

RiverNode::~RiverNode()
{
	//todo?
}

RiverBranch::RiverBranch(RiverNode * s, RiverNode * e)
	:start(s), end(e)
{
	id++;
}

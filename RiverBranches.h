#pragma once

#include <vec.h>
#include <vector>

// class of river node
class RiverNode
{
public:
	// node priority, used for node creation
	int priority;
	// node position
	vec3 position;
	// parent node
	RiverNode* parent;
	// children nodes
	vector<RiverNode*> children;
	//Terminal nodes or not
	bool terminal;

	RiverNode();
	RiverNode(int p, vec3 pos,RiverNode* parent);
	~RiverNode();

	//void updatePriority(int p);
	//int getPriority();
	//void getPosition(vec3 pos);
	//vec3 getPosition();
	//void setParent(RiverNode* p);
	//RiverNode* getParent();
	//void addChild(RiverNode* c);
	//RiverNode* getChild(int i);

};



// class of river branch
class RiverBranch
{
public:
	RiverNode* start;
	RiverNode* end;
	static int id;

	static int id;

	RiverBranch(RiverNode* s, RiverNode* e);
};
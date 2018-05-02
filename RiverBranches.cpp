// RiverBranches.cpp : Defines the entry point for the console application.
//



#include "RiverBranches.h"
#include "RiverNetwork.h"
#include <algorithm>
#include <fstream>


#define DoubleCheck 1e-5
extern vector<pair<int, int>> branchIndices(RiverBranch* branch, double step, bool pixelwise);

int RiverBranch::index = 0;
int RiverNode::index = 0;

RiverNode::RiverNode()
	:priority(1), position(vec3(0, 0, 0)), parent(nullptr), terminal(false)
{
	index++;
	id = index;
}


RiverNode::RiverNode(int p, vec3 pos, RiverNode* parent)
	: priority(p), position(pos), parent(parent), terminal(false)
{
	index++;
	id = index;
}

RiverNode::~RiverNode()
{
	//todo?
}

void RiverNode::setElevation(double z)
{
	position[2] = z;
}

RiverBranch::RiverBranch(RiverNode * s, RiverNode * e)
	:start(s), end(e)
{
	length = (s->position - e->position).Length();

	index++;
	id = index;

	branchType = 1;
}

//auxiliary functions and structures for check of line segment intersections
struct Point
{
	double x;
	double y;
};

// Given three colinear points p, q, r, the function checks if
// point q lies on line segment 'pr'
bool onSegment(Point p, Point q, Point r)
{
	if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
		q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
		return true;

	return false;
}

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are colinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int orientation(Point p, Point q, Point r)
{
	//cross product
	double val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

	if (abs(val) <= DoubleCheck) return 0;  // colinear

	return (val > 0) ? 1 : 2; // clock or counterclock wise
}

// The main function that returns true if line segment 'p1q1'
// and 'p2q2' intersect.
bool doIntersect(Point p1, Point q1, Point p2, Point q2)
{
	// Find the four orientations needed for general and
	// special cases
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	// General case
	if (o1 != o2 && o3 != o4)
		return true;

	// Special Cases
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1
	if (o1 == 0 && onSegment(p1, p2, q1)) return true;

	// p1, q1 and q2 are colinear and q2 lies on segment p1q1
	if (o2 == 0 && onSegment(p1, q2, q1)) return true;

	// p2, q2 and p1 are colinear and p1 lies on segment p2q2
	if (o3 == 0 && onSegment(p2, p1, q2)) return true;

	// p2, q2 and q1 are colinear and q1 lies on segment p2q2
	if (o4 == 0 && onSegment(p2, q1, q2)) return true;

	return false; // Doesn't fall in any of the above cases
}
double RiverBranch::distance(RiverBranch* branch) {

	//distance from this to branch
	double dis = DBL_MAX;

	RiverBranch* base = branch, *cmp = this;
	//check if there are any intersections
	struct Point tS = { cmp->start->position[0],cmp->start->position[1] }, tE = { cmp->end->position[0],cmp->end->position[1] };
	struct Point bS = { base->start->position[0],base->start->position[1] }, bE = { base->end->position[0],base->end->position[1] };
	if (doIntersect(tS, tE, bS, bE))
	{
		return 0.0;
	}
	//if no intersection, compute four(point, line segment)distance and get the smallest one
	int num = 0;
	while (num < 2) {

		double dis1 = DBL_MAX;

		//make sure that x-axis of start point is less than that of end point
		vec3 start = branch->start->position, end = branch->end->position;
		if (start[0] > end[0]) swap(start, end);

		//calculate direction of branch
		double dx = end[0] - start[0];
		double dy = end[1] - start[1];
		vec2 dir = vec2(dx, dy);
		double t = dir.Length();
		dir.Normalize();
		vec2 per = vec2(-dy, dx);
		per.Normalize();

		//calculate endpoints of this w.r.t branch start point
		//T->this branch
		//no T ->incoming branch
		vec3 startT = this->start->position, endT = this->end->position;
		if (startT[0] > endT[0]) swap(startT, endT);

		vec2 offsetS = vec2(startT[0] - start[0],
			startT[1] - start[1]);
		vec2 offsetE = vec2(endT[0] - start[0],
			endT[1] - start[1]);

		//t1 and t2 are projection on dir 
		//t is the length of incoming branh
		double t1 = Dot(offsetS, dir), t2 = Dot(offsetE, dir);
		if (t2 < 0.0) {
			dis1 = offsetE.Length();
		}
		else if (t1 > t) {
			vec2 d = vec2(startT[0] - end[0], startT[1] - end[1]);
			dis1 = d.Length();
		}
		else {
			//cout << "enter per, num = " << num << endl;
			if (t1 < 0.0) {
				dis1 = abs(Dot(offsetE, per));
			}
			else {
				dis1 = abs(Dot(offsetS, per));
			}
		}

		dis = min(dis, dis1);
		num++;
		swap(base, cmp);
	}

	return dis;

}

//get the elevation from bitmap
double RiverNode::getElevation(double H, double W, std::vector<vector<double>> &heightmap)
{
	int x = int(position[0]);
	int y = int(position[1]);
	x = x >= 1024 ? 1023 : x;
	y = y >= 1024 ? 1023 : y;
	x = x <= 0 ? 0 : x;
	y = y <= 0 ? 0 : y;
	return heightmap[x][y];
	//return 100.0 * exp(-pow(position[0] - H / 2.0, 2.0) / 200000 - pow(position[1] - W / 2.0, 2.0) / 200000);
}


// calculate the direction for river branch carving
vec2 RiverBranch::getCarveDirection() {

	// compute branch slope
	double dx = this->end->position[0] - this->start->position[0];
	double dy = this->end->position[1] - this->start->position[1];

	// if branch is vertical
	if (std::abs(dx) < DBL_EPSILON) {
		return vec2(1.0, 0.0);
	}

	// if branch is horizontal
	else if (std::abs(dy) < DBL_EPSILON) {
		return vec2(0.0, 1.0);
	}

	else {
		vec2 dir = vec2(-dy, dx);
		dir.Normalize();
		return dir;
	}
}

//// main function
//int main()
//{
//	RiverNetwork RN = RiverNetwork(1024, 1024, 30);
//	//this step would construct a 1024 x 1024 double heightMap by assigning the values in elevationMap
//	//RN.readBMP("./TestImage/gymHJ.bmp");
//	//RN.readBMP("./TestImage/HeightMap.bmp");
//	RN.readElevation("heightvalues.txt");
//	RN.initialNode();
//
//	for (int i = 0; i < 1000; i++)
//	{
//		RiverNode* cNode = RN.selectNode();
//		if (cNode == nullptr)
//		{
//			break;
//		}
//		RN.expandNode(cNode);
//	}
//	//write the river results into a new bitmap file
//	RN.writeRivers("./TestImage/HeightMap.bmp");
//
//	RN.carveRiver();
//	RN.writeRiversFromElevation("./TestImage/HeightMap.bmp");
//
//	return 0;
//}


#include "LSystemNode.h"

#include <maya/MTime.h>
#include <maya/MFnMesh.h>
#include <maya/MPoint.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnPlugin.h>

#include <maya/MPxNode.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MFnMeshData.h>

#include <maya/MIOStream.h>

#include "LSystem.h"
#include "cylinder.h"
typedef std::pair<vec3, vec3> Branch;
#define McheckErr(stat,msg)			\
	if ( MS::kSuccess != stat ) {	\
		cerr << msg;				\
		return MS::kFailure;		\
	}

MObject LSystemNode::time;
MObject LSystemNode::outputMesh;
MObject LSystemNode::dfAngle;
MObject LSystemNode::dfStepsize;
MObject LSystemNode::grammar;
MTypeId LSystemNode::nodeID(0x80000);

MString LSystemNode::grammarPath;

LSystemNode::LSystemNode()
{
}


LSystemNode::~LSystemNode()
{
}

void* LSystemNode::creator()
{
	return new LSystemNode;
}

MStatus LSystemNode::initialize()
{
	MFnNumericAttribute numAttr;
	MFnNumericAttribute numAttr2;
	MFnTypedAttribute typeAttr;
	MFnTypedAttribute typeAttr2;
	MFnUnitAttribute unitAttr;

	MStatus returnStatus;

	//create Attributes
	LSystemNode::time = unitAttr.create("time", "tm",
		MFnUnitAttribute::kTime, 0.0, &returnStatus);
	McheckErr(returnStatus, "ERROR creating lsys time attribute\n");

	LSystemNode::outputMesh = typeAttr.create("outputMesh", "out",
		MFnData::kMesh, &returnStatus);
	McheckErr(returnStatus, "ERROR creating lsys output attribute\n");
	LSystemNode::dfAngle = numAttr.create("defaultAngle", "da",
		MFnNumericData::kDouble, 0.0, &returnStatus);
	McheckErr(returnStatus, "ERROR creating lsys angle attribute\n");
	LSystemNode::dfStepsize = numAttr2.create("defaultStepSize", "ds",
		MFnNumericData::kDouble, 0.0, &returnStatus);
	McheckErr(returnStatus, "ERROR creating lsys stepsize attribute\n");
	LSystemNode::grammar = typeAttr2.create("grammar", "gm",
		MFnData::kString, &returnStatus);
	McheckErr(returnStatus, "ERROR creating lsys grammar attribute\n");

	//add attributes
	typeAttr.setStorable(false);
	returnStatus = addAttribute(LSystemNode::time);
	McheckErr(returnStatus, "ERROR adding time attribute\n");
	returnStatus = addAttribute(LSystemNode::outputMesh);
	McheckErr(returnStatus, "ERROR adding mesh attribute\n");
	returnStatus = addAttribute(LSystemNode::dfAngle);
	McheckErr(returnStatus, "ERROR adding angle attribute\n");
	returnStatus = addAttribute(LSystemNode::dfStepsize);
	McheckErr(returnStatus, "ERROR adding stepsize attribute\n");
	returnStatus = addAttribute(LSystemNode::grammar);
	McheckErr(returnStatus, "ERROR adding grammar attribute\n");

	//connecting attributes
	returnStatus = attributeAffects(LSystemNode::time,
		LSystemNode::outputMesh);
	McheckErr(returnStatus, "ERROR connecting attribute\n");
	returnStatus = attributeAffects(LSystemNode::dfAngle,
		LSystemNode::outputMesh);
	McheckErr(returnStatus, "ERROR connecting attribute\n");
	returnStatus = attributeAffects(LSystemNode::dfStepsize,
		LSystemNode::outputMesh);
	McheckErr(returnStatus, "ERROR connecting attribute\n");
	returnStatus = attributeAffects(LSystemNode::grammar,
		LSystemNode::outputMesh);
	McheckErr(returnStatus, "ERROR connecting attribute\n");
	return MS::kSuccess;
}

MObject LSystemNode::createMesh(const MTime & time, const double & angle, 
	const double & stepsize, MString & grammar, 
	MObject & outData, MStatus & stat)
{
	std::vector<Branch>branches;
	LSystem lsys;
	lsys.loadProgram(grammar.asChar());
	lsys.setDefaultAngle(angle);
	lsys.setDefaultStep(stepsize);
	int iterationNum = (int)time.as(MTime::kFilm);


	////////////////////////////////////
    lsys.process(iterationNum, branches);

	MPointArray points;
	MIntArray faceCounts;
	MIntArray faceConnects;
	for (int i = 0; i < branches.size(); i++)
	{
		vec3 startP = branches[i].first;
		vec3 endP = branches[i].second;
		MPoint startMP = MPoint(startP[0], startP[1], startP[2]);
		MPoint endMP = MPoint(endP[0], endP[1], endP[2]);
		CylinderMesh branchCylin = CylinderMesh(startMP, endMP);
		branchCylin.appendToMesh(points, faceCounts, faceConnects);
	}
	MFnMesh meshFS;
	MObject newMesh = meshFS.create(points.length(), faceCounts.length(), points, faceCounts, faceConnects, outData, &stat);
	return newMesh;
}

MStatus LSystemNode::compute(const MPlug& plug, MDataBlock& data)
{
	MStatus returnStatus;
	if (plug == outputMesh) {
		//get time
		MDataHandle timeData = data.inputValue(time, &returnStatus);
		MTime time = timeData.asTime();

		//get default angle
		MDataHandle angleData = data.inputValue(dfAngle, &returnStatus);
		double angle = angleData.asDouble();
		//get default stepsize
		MDataHandle stepsizeData = data.inputValue(dfStepsize, &returnStatus);
		double stepsize = stepsizeData.asDouble();
		//get grammar
		MDataHandle grammarData = data.inputValue(grammar,&returnStatus);
		MString grammarFile = LSystemNode::grammarPath + "\\plants\\"+ grammarData.asString();
		//get output object
		MDataHandle outputHandle = data.outputValue(outputMesh, &returnStatus);

		MFnMeshData dataCreator;
		MObject newOutputData = dataCreator.create(&returnStatus);

		createMesh(time, angle, stepsize, grammarFile, newOutputData, returnStatus);
		outputHandle.set(newOutputData);
		data.setClean(plug);
	}
	return MStatus::kSuccess;
}

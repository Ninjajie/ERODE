#include "ERODENode.h"

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
//#include <maya/MFnMeshData.h>
#include <maya/MFnStringData.h>
#include <maya/MIOStream.h>

#include "RiverBranches.h"
#include "RiverNetwork.h"
#include <maya/MGlobal.h>

#include <chrono>
#include <thread>

#include <fstream>
#include "bitmap_image.hpp"

#define McheckErr(stat,msg)			\
	if ( MS::kSuccess != stat ) {	\
		cerr << msg;				\
		return MS::kFailure;		\
	}

MObject ERODENode::time;

MObject ERODENode::dfAngle;
MObject ERODENode::dfStepsize;
MObject ERODENode::grammar;
MTypeId ERODENode::nodeID(0x80000);
//erode
MObject ERODENode::carve;
MObject ERODENode::outputPath;
MObject ERODENode::inputPath;

MString ERODENode::grammarPath;

ERODENode::ERODENode()
{
}


ERODENode::~ERODENode()
{
}

void* ERODENode::creator()
{
	return new ERODENode;
}

MStatus ERODENode::initialize()
{
	MFnNumericAttribute numAttr;
	MFnNumericAttribute numAttr2;
	MFnTypedAttribute typeAttr;
	MFnTypedAttribute typeAttr2;
	MFnUnitAttribute unitAttr;

	//MFnStringData oString;

	MStatus returnStatus;
	//MStatus createStatus;
//////////////////////////////////////////////////
	//create Attributes
	ERODENode::time = unitAttr.create("time", "tm",
		MFnUnitAttribute::kTime, 0.0, &returnStatus);
	McheckErr(returnStatus, "ERROR creating lsys time attribute\n");

	ERODENode::dfAngle = numAttr.create("defaultAngle", "da",
		MFnNumericData::kDouble, 0.0, &returnStatus);
	McheckErr(returnStatus, "ERROR creating lsys angle attribute\n");
	ERODENode::dfStepsize = numAttr2.create("defaultStepSize", "ds",
		MFnNumericData::kDouble, 0.0, &returnStatus);
	McheckErr(returnStatus, "ERROR creating lsys stepsize attribute\n");
	ERODENode::grammar = typeAttr2.create("grammar", "gm",
		MFnData::kString, &returnStatus);
	McheckErr(returnStatus, "ERROR creating lsys grammar attribute\n");

	//ERODEATTRS
	ERODENode::carve = numAttr.create("carve", "ca",
		MFnNumericData::kBoolean, false, &returnStatus);
	McheckErr(returnStatus, "ERROR creating erode carve attribute\n");

	ERODENode::inputPath = typeAttr.create("inputPath", "ip",
		MFnData::kString, &returnStatus);
	McheckErr(returnStatus, "ERROR creating erode inputpath attribute\n");

	ERODENode::outputPath = typeAttr.create("outputPath", "op",
		MFnData::kString, &returnStatus);
	McheckErr(returnStatus, "ERROR creating erode outputpath attribute\n");
///////////////////////////////////////////////////
	//add attributes
	typeAttr.setStorable(false);
	returnStatus = addAttribute(ERODENode::time);
	McheckErr(returnStatus, "ERROR adding time attribute\n");

	returnStatus = addAttribute(ERODENode::dfAngle);
	McheckErr(returnStatus, "ERROR adding angle attribute\n");
	returnStatus = addAttribute(ERODENode::dfStepsize);
	McheckErr(returnStatus, "ERROR adding stepsize attribute\n");
	returnStatus = addAttribute(ERODENode::grammar);
	McheckErr(returnStatus, "ERROR adding grammar attribute\n");

	//ERODEATTRS
	returnStatus = addAttribute(ERODENode::carve);
	McheckErr(returnStatus, "ERROR adding carve attribute\n");
	returnStatus = addAttribute(ERODENode::inputPath);
	McheckErr(returnStatus, "ERROR adding inputPath attribute\n");
	returnStatus = addAttribute(ERODENode::outputPath);
	McheckErr(returnStatus, "ERROR adding outputPath attribute\n");
///////////////////////////////////////////
	//connecting attributes


	//erodearrts
	returnStatus = attributeAffects(ERODENode::inputPath,
		ERODENode::outputPath);
	McheckErr(returnStatus, "ERROR connecting attribute\n");
	returnStatus = attributeAffects(ERODENode::carve,
		ERODENode::outputPath);
	McheckErr(returnStatus, "ERROR connecting attribute\n");
	return MS::kSuccess;
}

void ERODENode::computeImage(std::string& carved, std::string& carvedSmoothed)
{
	//create River Network and carve
	RiverNetwork RN = RiverNetwork(1024, 1024, 30);
	//this step would construct a 1024 x 1024 double heightMap by assigning the values in elevationMap
	//RN.readBMP("./TestImage/gymHJ.bmp");
	//RN.readBMP("./TestImage/HeightMap.bmp");
	RN.readElevation("c:/Test/heightvalues.txt");
	RN.initialNode();

	for (int i = 0; i < 100; i++)
	{
		RiverNode* cNode = RN.selectNode();
		if (cNode == nullptr)
		{
			break;
		}
		RN.expandNode(cNode);
		MGlobal::displayInfo(MString("looping"));
	}
	//write the river results into a new bitmap file

	RN.writeRivers("c:/Test/HeightMap.bmp");
	MGlobal::displayInfo(MString("writing?"));
	RN.carveRiver();
	RN.writeRiversFromElevation("c:/Test/HeightMap.bmp", carved, carvedSmoothed);
	//std::this_thread::sleep_for(std::chrono::seconds(70));
	MGlobal::displayInfo(MString("writing?"));
	
}

MStatus ERODENode::compute(const MPlug& plug, MDataBlock& data)
{
	MStatus returnStatus;
	if (plug == outputPath) {

		//first of all, get the carve value, only compute if it is true
		MDataHandle carveOrNot = data.inputValue(carve, &returnStatus);
		bool carveValue = carveOrNot.asBool();
		if (carveValue == true)
		{
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
			MDataHandle grammarData = data.inputValue(grammar, &returnStatus);
			MString grammarFile = ERODENode::grammarPath + "\\plants\\" + grammarData.asString();

			//get input Path
			MDataHandle imagePath = data.inputValue(inputPath, &returnStatus);
			MString imageFile = imagePath.asString();
			
			//RN.writeRivers(imageFile.asChar());
			//RN.carveRiver();
			//RN.writeRiversFromElevation(imageFile.asChar());

			std::string carvedpath;
			std::string carvedSmoothedpath;

			//computeImage(carvedpath, carvedSmoothedpath);
			MImage rawimage;
			rawimage.readFromFile("c:/Test/Heightmap.jpg");
			unsigned int width, height;
			rawimage.getSize(width, height);
			unsigned int imagesize = width * height;

			unsigned char *pixels = rawimage.pixels();
			unsigned int i;
			for (i = 0; i < imagesize; i++)
			{
				if (i <= imagesize/2) {
					*(pixels++) = (unsigned char)(0);
					*(pixels++) = (unsigned char)(0);
					*(pixels++) = (unsigned char)(0);
					pixels++;
				}
			}
			rawimage.writeToFile("c:/Test/Heighttest.jpg", "jpg");


			//////customize output/////////////////
			MDataHandle outputHandle = data.outputValue(outputPath, &returnStatus);
			MString outfilepath = MString("c:/Test/Heighttest.jpg");
			//MString outfilepath = MString(carvedpath.c_str());
			outputHandle.set(outfilepath);

			data.setClean(plug);
			return MS::kSuccess;
		}
		else
		{
			//get input Path
			MDataHandle imagePath = data.inputValue(inputPath, &returnStatus);
			MString imageFile = imagePath.asString();
			MDataHandle outputHandle = data.outputValue(outputPath, &returnStatus);
			outputHandle.set(imageFile);
			data.setClean(plug);
			return MStatus::kSuccess;
		}	
	}
}


void ERODENode::populateTestImage(float* pixels, unsigned int w, unsigned int h)
{
#define RAINBOW_SCALE 4.0f
	unsigned int x, y;
	for (y = 0; y < h; y++)
	{
		float g = RAINBOW_SCALE * y / (float)h;
		for (x = 0; x < w; x++)
		{
			float r = RAINBOW_SCALE * x / (float)w;
			*pixels++ = r;
			*pixels++ = g;
			*pixels++ = RAINBOW_SCALE * 1.5f - r - g;
		}
	}
}


//
// DESCRIPTION:
// Load the contents of this image file into an MImage. A real
// file format plugin would extract the pixel data from the image
// file here.
//
MStatus ERODENode::load(MImage& image, unsigned int)
{
	unsigned int w = 512;
	unsigned int h = 512;

	// Create a floating point image and fill it with
	// a pretty rainbow test image.
	//
	image.create(w, h, 3, MImage::kFloat);
	populateTestImage(image.floatPixels(), w, h);
	return MS::kSuccess;
}
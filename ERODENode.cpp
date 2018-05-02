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


#include <fstream>
#include "bitmap_image.hpp"

#define McheckErr(stat,msg)			\
	if ( MS::kSuccess != stat ) {	\
		cerr << msg;				\
		return MS::kFailure;		\
	}

MObject ERODENode::time;
MTypeId ERODENode::nodeID(0x80001);
//erode
MObject ERODENode::carve;
MObject ERODENode::smooth;
MObject ERODENode::outputPath;
MObject ERODENode::inputPath;

MObject ERODENode::branchLength;
MObject ERODENode::elevationPercentile;
MObject ERODENode::distanceRatio;
MObject ERODENode::probabilityS;
MObject ERODENode::probabilityA;
MObject ERODENode::riverWidth;
MObject ERODENode::riverHeight;
MObject ERODENode::branchWidth;
MObject ERODENode::elevationRange;

MObject ERODENode::mouthOnFirstEdge;
MObject ERODENode::firstMouthRatio;
MObject ERODENode::mouthOnSecondEdge;
MObject ERODENode::secondMouthRatio;
MObject ERODENode::mouthOnThirdEdge;
MObject ERODENode::thirdMouthRatio;
MObject ERODENode::mouthOnFourthEdge;
MObject ERODENode::fourthMouthRatio;
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
	MStatus returnStatus;
//////////////////////////////////////////////////
	//create Attributes
	//mouth attrs
	ERODENode::mouthOnFirstEdge = numAttr.create("mouthOnFirstEdge", "fm",
		MFnNumericData::kBoolean, true, &returnStatus);
	McheckErr(returnStatus, "ERROR creating erode attribute\n");
	ERODENode::firstMouthRatio = numAttr.create("firstMouthRatio", "fr",
		MFnNumericData::kDouble, -1.0, &returnStatus);
	McheckErr(returnStatus, "ERROR creating erode attribute\n");

	ERODENode::mouthOnSecondEdge = numAttr.create("mouthOnSecondEdge", "sm",
		MFnNumericData::kBoolean, true, &returnStatus);
	McheckErr(returnStatus, "ERROR creating erode attribute\n");
	ERODENode::secondMouthRatio = numAttr.create("secondMouthRatio", "sr",
		MFnNumericData::kDouble, -1.0, &returnStatus);
	McheckErr(returnStatus, "ERROR creating erode attribute\n");

	ERODENode::mouthOnThirdEdge = numAttr.create("mouthOnThirdEdge", "tm",
		MFnNumericData::kBoolean, true, &returnStatus);
	McheckErr(returnStatus, "ERROR creating erode attribute\n");
	ERODENode::thirdMouthRatio = numAttr.create("thirdMouthRatio", "tr",
		MFnNumericData::kDouble, -1.0, &returnStatus);
	McheckErr(returnStatus, "ERROR creating erode attribute\n");

	ERODENode::mouthOnFourthEdge = numAttr.create("mouthOnFourthEdge", "om",
		MFnNumericData::kBoolean, true, &returnStatus);
	McheckErr(returnStatus, "ERROR creating erode attribute\n");
	ERODENode::fourthMouthRatio = numAttr.create("fourthMouthRatio", "or",
		MFnNumericData::kDouble, -1.0, &returnStatus);
	McheckErr(returnStatus, "ERROR creating erode attribute\n");

	//ERODEATTRS
	ERODENode::branchLength = numAttr.create("branchLength", "bl",
		MFnNumericData::kDouble, 30.0, &returnStatus);
	McheckErr(returnStatus, "ERROR creating erode carve attribute\n");
	ERODENode::elevationPercentile = numAttr.create("elevationPercentile", "ep",
		MFnNumericData::kDouble, 0.05, &returnStatus);
	McheckErr(returnStatus, "ERROR creating erode attribute\n");
	ERODENode::distanceRatio = numAttr.create("distanceRatio", "dr",
		MFnNumericData::kDouble, 0.75, &returnStatus);
	McheckErr(returnStatus, "ERROR creating erode attribute\n");
	ERODENode::probabilityS = numAttr.create("probabilityS", "ps",
		MFnNumericData::kDouble, 0.7, &returnStatus);
	McheckErr(returnStatus, "ERROR creating erode attribute\n");
	ERODENode::probabilityA = numAttr.create("probabilityA", "pa",
		MFnNumericData::kDouble, 0.2, &returnStatus);
	McheckErr(returnStatus, "ERROR creating erode attribute\n");
	ERODENode::riverWidth = numAttr.create("riverWidth", "rw",
		MFnNumericData::kDouble, 1.0, &returnStatus);
	McheckErr(returnStatus, "ERROR creating erode attribute\n");
	ERODENode::riverHeight = numAttr.create("riverHeight", "rh",
		MFnNumericData::kDouble, 10.0, &returnStatus);
	McheckErr(returnStatus, "ERROR creating erode attribute\n");
	ERODENode::branchWidth = numAttr.create("branchWidth", "bw",
		MFnNumericData::kDouble, 5.0, &returnStatus);
	McheckErr(returnStatus, "ERROR creating erode attribute\n");

	ERODENode::elevationRange = numAttr.create("elevationRange", "er",
		MFnNumericData::kDouble, 25.0, &returnStatus);
	McheckErr(returnStatus, "ERROR creating erode attribute\n");


	ERODENode::carve = numAttr.create("carve", "ca",
		MFnNumericData::kBoolean, false, &returnStatus);
	McheckErr(returnStatus, "ERROR creating erode attribute\n");
	//ERODENode::smooth = numAttr.create("smooth", "sm",
	//	MFnNumericData::kBoolean, false, &returnStatus);
	//McheckErr(returnStatus, "ERROR creating erode attribute\n");


	ERODENode::inputPath = typeAttr.create("inputPath", "ip",
		MFnData::kString, &returnStatus);
	McheckErr(returnStatus, "ERROR creating erode inputpath attribute\n");

	ERODENode::outputPath = typeAttr.create("outputPath", "op",
		MFnData::kString, &returnStatus);
	McheckErr(returnStatus, "ERROR creating erode outputpath attribute\n");
///////////////////////////////////////////////////
	//add attributes
	typeAttr.setStorable(false);
	//initial mouth attrs
	returnStatus = addAttribute(ERODENode::firstMouthRatio);
	McheckErr(returnStatus, "ERROR adding attribute\n");
	returnStatus = addAttribute(ERODENode::mouthOnFirstEdge);
	McheckErr(returnStatus, "ERROR adding attribute\n");

	returnStatus = addAttribute(ERODENode::secondMouthRatio);
	McheckErr(returnStatus, "ERROR adding attribute\n");
	returnStatus = addAttribute(ERODENode::mouthOnSecondEdge);
	McheckErr(returnStatus, "ERROR adding attribute\n");

	returnStatus = addAttribute(ERODENode::thirdMouthRatio);
	McheckErr(returnStatus, "ERROR adding attribute\n");
	returnStatus = addAttribute(ERODENode::mouthOnThirdEdge);
	McheckErr(returnStatus, "ERROR adding attribute\n");

	returnStatus = addAttribute(ERODENode::fourthMouthRatio);
	McheckErr(returnStatus, "ERROR adding attribute\n");
	returnStatus = addAttribute(ERODENode::mouthOnFourthEdge);
	McheckErr(returnStatus, "ERROR adding attribute\n");

	//ERODEATTRS
	returnStatus = addAttribute(ERODENode::branchLength);
	McheckErr(returnStatus, "ERROR adding attribute\n");
	returnStatus = addAttribute(ERODENode::elevationPercentile);
	McheckErr(returnStatus, "ERROR adding attribute\n");
	returnStatus = addAttribute(ERODENode::distanceRatio);
	McheckErr(returnStatus, "ERROR adding attribute\n");
	returnStatus = addAttribute(ERODENode::probabilityS);
	McheckErr(returnStatus, "ERROR adding attribute\n");
	returnStatus = addAttribute(ERODENode::probabilityA);
	McheckErr(returnStatus, "ERROR adding attribute\n");
	returnStatus = addAttribute(ERODENode::riverWidth);
	McheckErr(returnStatus, "ERROR adding attribute\n");
	returnStatus = addAttribute(ERODENode::riverHeight);
	McheckErr(returnStatus, "ERROR adding attribute\n");
	returnStatus = addAttribute(ERODENode::branchWidth);
	McheckErr(returnStatus, "ERROR adding attribute\n");
	returnStatus = addAttribute(ERODENode::elevationRange);
	McheckErr(returnStatus, "ERROR adding attribute\n");

	returnStatus = addAttribute(ERODENode::carve);
	McheckErr(returnStatus, "ERROR adding carve attribute\n");
	//returnStatus = addAttribute(ERODENode::smooth);
	//McheckErr(returnStatus, "ERROR adding smooth attribute\n");

	returnStatus = addAttribute(ERODENode::inputPath);
	McheckErr(returnStatus, "ERROR adding inputPath attribute\n");
	returnStatus = addAttribute(ERODENode::outputPath);
	McheckErr(returnStatus, "ERROR adding outputPath attribute\n");
///////////////////////////////////////////
	//connecting attributes
	//initial mouth attributes
	returnStatus = attributeAffects(ERODENode::mouthOnFirstEdge,
		ERODENode::outputPath);
	McheckErr(returnStatus, "ERROR connecting attribute\n");
	returnStatus = attributeAffects(ERODENode::firstMouthRatio,
		ERODENode::outputPath);
	McheckErr(returnStatus, "ERROR connecting attribute\n");

	returnStatus = attributeAffects(ERODENode::mouthOnSecondEdge,
		ERODENode::outputPath);
	McheckErr(returnStatus, "ERROR connecting attribute\n");
	returnStatus = attributeAffects(ERODENode::secondMouthRatio,
		ERODENode::outputPath);
	McheckErr(returnStatus, "ERROR connecting attribute\n");

	returnStatus = attributeAffects(ERODENode::mouthOnThirdEdge,
		ERODENode::outputPath);
	McheckErr(returnStatus, "ERROR connecting attribute\n");
	returnStatus = attributeAffects(ERODENode::thirdMouthRatio,
		ERODENode::outputPath);
	McheckErr(returnStatus, "ERROR connecting attribute\n");

	returnStatus = attributeAffects(ERODENode::mouthOnFourthEdge,
		ERODENode::outputPath);
	McheckErr(returnStatus, "ERROR connecting attribute\n");
	returnStatus = attributeAffects(ERODENode::fourthMouthRatio,
		ERODENode::outputPath);
	McheckErr(returnStatus, "ERROR connecting attribute\n");


	//erodearrts
	returnStatus = attributeAffects(ERODENode::branchLength,
		ERODENode::outputPath);
	McheckErr(returnStatus, "ERROR connecting attribute\n");
	returnStatus = attributeAffects(ERODENode::elevationPercentile,
		ERODENode::outputPath);
	McheckErr(returnStatus, "ERROR connecting attribute\n");
	returnStatus = attributeAffects(ERODENode::distanceRatio,
		ERODENode::outputPath);
	McheckErr(returnStatus, "ERROR connecting attribute\n");
	returnStatus = attributeAffects(ERODENode::probabilityS,
		ERODENode::outputPath);
	McheckErr(returnStatus, "ERROR connecting attribute\n");
	returnStatus = attributeAffects(ERODENode::probabilityA,
		ERODENode::outputPath);
	McheckErr(returnStatus, "ERROR connecting attribute\n");
	returnStatus = attributeAffects(ERODENode::riverWidth,
		ERODENode::outputPath);
	McheckErr(returnStatus, "ERROR connecting attribute\n");
	returnStatus = attributeAffects(ERODENode::riverHeight,
		ERODENode::outputPath);
	McheckErr(returnStatus, "ERROR connecting attribute\n");
	returnStatus = attributeAffects(ERODENode::branchWidth,
		ERODENode::outputPath);
	McheckErr(returnStatus, "ERROR connecting attribute\n");
	returnStatus = attributeAffects(ERODENode::inputPath,
		ERODENode::outputPath);
	McheckErr(returnStatus, "ERROR connecting attribute\n");

	returnStatus = attributeAffects(ERODENode::elevationRange,
		ERODENode::outputPath);
	McheckErr(returnStatus, "ERROR connecting attribute\n");

	returnStatus = attributeAffects(ERODENode::carve,
		ERODENode::outputPath);
	McheckErr(returnStatus, "ERROR connecting attribute\n");
	//returnStatus = attributeAffects(ERODENode::smooth,
	//	ERODENode::outputPath);
	McheckErr(returnStatus, "ERROR connecting attribute\n");
	return MS::kSuccess;
}

void ERODENode::computeImage(std::string originFile, std::string& altered,
	std::string& carved, std::string& carvedSmoothed,
	double branchLength, double elevationPercentile, double distanceRatio,
	double probabilityS, double probabilityA,
	double riverWidth, double riverHeight, double branchWidth,
	double elevationRange,
	bool mouth1, double ratio1,
	bool mouth2, double ratio2, 
	bool mouth3, double ratio3, 
	bool mouth4, double ratio4 )
{
	//create River Network and carve
	RiverNetwork RN = RiverNetwork(1024, 1024, 
		branchLength,5.0,elevationPercentile,distanceRatio,
		probabilityS, probabilityA,
		riverWidth, riverHeight, branchWidth);
	//this step would construct a 1024 x 1024 double heightMap by assigning the values in elevationMap
	//RN.readBMP("./TestImage/gymHJ.bmp");
	RN.readBMP(originFile);
	//RN.readElevation("c:/Test/heightvalues.txt");
	RN.elevationRange = elevationRange;
	RN.initialNode(mouth1,ratio1,mouth2,ratio2,mouth3,ratio3,mouth4,ratio4);

	for (int i = 0; i < 1000; i++)
	{
		RiverNode* cNode = RN.selectNode();
		if (cNode == nullptr)
		{
			break;
		}
		RN.expandNode(cNode);
	}
	//write the river results into a new file

	RN.writeRivers(originFile, altered);
	RN.carveRiver();
	RN.writeRiversFromElevation(originFile, carved, carvedSmoothed);
	
}

MStatus ERODENode::compute(const MPlug& plug, MDataBlock& data)
{
	MStatus returnStatus;
	if (plug == outputPath) {

		//first of all, get the carve value and smooth value, only compute if it is true
		MDataHandle carveOrNot = data.inputValue(carve, &returnStatus);
		bool carveValue = carveOrNot.asBool();

		if (carveValue == true)
		{
			//get initial mouth attributes
			MDataHandle firstmouth = data.inputValue(mouthOnFirstEdge, &returnStatus);
			bool mouth1 = firstmouth.asBool();
			MDataHandle firstRatio = data.inputValue(firstMouthRatio, &returnStatus);
			double ratio1 = firstRatio.asDouble();

			MDataHandle secondmouth = data.inputValue(mouthOnSecondEdge, &returnStatus);
			bool mouth2 = secondmouth.asBool();
			MDataHandle secondRatio = data.inputValue(secondMouthRatio, &returnStatus);
			double ratio2 = secondRatio.asDouble();

			MDataHandle thirdmouth = data.inputValue(mouthOnThirdEdge, &returnStatus);
			bool mouth3 = thirdmouth.asBool();
			MDataHandle thirdRatio = data.inputValue(thirdMouthRatio, &returnStatus);
			double ratio3 = thirdRatio.asDouble();

			MDataHandle fourthmouth = data.inputValue(mouthOnFourthEdge, &returnStatus);
			bool mouth4 = fourthmouth.asBool();
			MDataHandle fourthRatio = data.inputValue(fourthMouthRatio, &returnStatus);
			double ratio4 = fourthRatio.asDouble();

			//get erode attributes
			MDataHandle branchLengthData = data.inputValue(branchLength, &returnStatus);
			double branchlength = branchLengthData.asDouble();

			MDataHandle elevationPercentileData = data.inputValue(elevationPercentile, &returnStatus);
			double elevationpercentile = elevationPercentileData.asDouble();

			MDataHandle distanceRatioData = data.inputValue(distanceRatio, &returnStatus);
			double distanceratio = distanceRatioData.asDouble();

			MDataHandle probabilitySData = data.inputValue(probabilityS, &returnStatus);
			double probabilitys = probabilitySData.asDouble();

			MDataHandle probabilityAData = data.inputValue(probabilityA, &returnStatus);
			double probabilitya = probabilityAData.asDouble();

			MDataHandle riverWidthData = data.inputValue(riverWidth, &returnStatus);
			double riverwidth = riverWidthData.asDouble();

			MDataHandle riverHeightData = data.inputValue(riverHeight, &returnStatus);
			double riverheight = riverHeightData.asDouble();

			MDataHandle branchWidthData = data.inputValue(branchWidth, &returnStatus);
			double branchwidth = branchWidthData.asDouble();

			//get input Path
			MDataHandle imagePath = data.inputValue(inputPath, &returnStatus);
			MString imageFile = imagePath.asString();
			

			std::string altered;
			std::string carvedpath;
			std::string carvedSmoothedpath;

			computeImage(imageFile.asChar(),altered, carvedpath, carvedSmoothedpath,
				branchlength,elevationpercentile,distanceratio,probabilitys,probabilitya,
				riverwidth,riverheight,branchwidth,
				25.0,
				mouth1,ratio1,mouth2,ratio2,mouth3,ratio3,mouth4,ratio4);


			//////customize output/////////////////
			MDataHandle outputHandle = data.outputValue(outputPath, &returnStatus);
			MString outfilepath;
			outfilepath = MString(carvedpath.c_str());
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



//std::string RiverNetwork::writeRivers(const std::string filename)
//{
//	bitmap_image originHeightmap(filename);
//	int w = originHeightmap.width();
//	int h = originHeightmap.height();
//	bitmap_image alteredHeightmap(w, h);
//	//first we just copy the original image
//	for (unsigned int y = 0; y < h; ++y)
//	{
//		for (unsigned int x = 0; x < w; ++x)
//		{
//			rgb_t colour;
//			originHeightmap.get_pixel(x, y, colour);
//			alteredHeightmap.set_pixel(x, y, colour);
//		}
//	}
//	//then for all the branches, we alter the corresponding pixel values
//	for (auto branch : this->branches)
//	{
//		//here for the only time we pass true to the pixelwise argument
//
//		std::vector<pair<int, int>> corespondPixels = branchIndices(branch, NEEDW, e, BranchWidth, DisRatio, 1.0, true);
//		for (auto indices : corespondPixels)
//		{
//			rgb_t newColor;
//			newColor.red = newColor.green = newColor.blue = static_cast<unsigned char>(0);
//			alteredHeightmap.set_pixel(indices.first, indices.second, newColor);
//		}
//	}
//
//	string outputFilename = filename;
//
//	std::size_t pos = outputFilename.find(".bmp");
//
//	outputFilename = outputFilename.substr(0, pos);
//
//	outputFilename += "Altered.bmp";
//
//	alteredHeightmap.save_image(outputFilename);
//	return outputFilename;
//}
//
//
//void RiverNetwork::writeRiversFromElevation(const std::string filename, std::string& carved, std::string carvedSmoothed) {
//	bitmap_image omap(filename);
//	int w = omap.width();
//	int h = omap.height();
//	bitmap_image map(w, h);
//	for (unsigned int i = 0; i < w; ++i) {
//		for (unsigned int j = 0; j < h; ++j) {
//			rgb_t color;
//			color.red = color.green = color.blue = static_cast<unsigned char>(elevationMap[j][i]);
//			map.set_pixel(i, j, color);
//		}
//	}
//
//	string outputFilename = filename;
//
//	std::size_t pos = outputFilename.find(".bmp");
//
//	outputFilename = outputFilename.substr(0, pos);
//
//	string o1 = outputFilename;
//	o1 += "Carved.bmp";
//
//	map.save_image(o1);
//
//	bitmap_image maps(w, h);
//	for (unsigned int i = 0; i < w; ++i) {
//		for (unsigned int j = 0; j < h; ++j) {
//			rgb_t color;
//			color.red = color.green = color.blue = static_cast<unsigned char>(elevationMapSmoothed[j][i]);
//			maps.set_pixel(i, j, color);
//		}
//	}
//
//	string o2 = outputFilename;
//	o2 += "CarvedSmoothed.bmp";
//
//	maps.save_image(o2);
//
//	carved = o1;
//	carvedSmoothed = o2;
//	return;
//}


//MImage rawimage;
//rawimage.readFromFile("c:/Test/Heightmap.jpg");
//unsigned int width, height;
//rawimage.getSize(width, height);
//unsigned int imagesize = width * height;

//unsigned char *pixels = rawimage.pixels();
//unsigned int i;
//for (i = 0; i < imagesize; i++)
//{
//	if (i <= imagesize/2) {
//		*(pixels++) = (unsigned char)(0);
//		*(pixels++) = (unsigned char)(0);
//		*(pixels++) = (unsigned char)(0);
//		pixels++;
//	}
//}
//rawimage.writeToFile("c:/Test/Heighttest.jpg", "jpg");
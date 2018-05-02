#define MNoVersionString 
#define MNoPluginEntry
#include <maya/MFnPlugin.h>
#include <maya/MImage.h>

class ERODENode :
	public MPxNode
{
public:
	ERODENode();
	virtual ~ERODENode();
	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	static void* creator();
	static MStatus initialize();

	static MTypeId nodeID;
	static MObject time;

	//erode
	static MObject carve;
	static MObject smooth;
	static MObject outputPath;
	static MObject inputPath;
	//parameters
	static MObject branchLength;
	static MObject elevationPercentile;
	static MObject distanceRatio;
	static MObject probabilityS;
	static MObject probabilityA;
	static MObject riverWidth;
	static MObject riverHeight;
	static MObject branchWidth;

	static MObject elevationRange;
	//parameters for initial nodes creation

	static MObject mouthOnFirstEdge;
	static MObject firstMouthRatio;
	static MObject mouthOnSecondEdge;
	static MObject secondMouthRatio;
	static MObject mouthOnThirdEdge;
	static MObject thirdMouthRatio;
	static MObject mouthOnFourthEdge;
	static MObject fourthMouthRatio;

protected:
	void computeImage(std::string originFile, 
		std::string& altered, 
		std::string& carved, 
		std::string& carvedSmoothed,
		double branchLength, double elevationPercentile, double distanceRatio, 
		double probabilityS, double probabilityA,
		double riverWidth, double riverHeight, double branchWidth,
		double elevationRange,
		bool mouth1, double ratio1,
		bool mouth2, double ratio2,
		bool mouth3, double ratio3,
		bool mouth4, double ratio4);
};


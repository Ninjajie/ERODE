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
	void populateTestImage(float * pixels, unsigned int w, unsigned int h);
	MStatus load(MImage & image, unsigned int);
	static void* creator();
	static MStatus initialize();

	static MTypeId nodeID;
	static MObject time;

	static MObject dfAngle;
	static MObject dfStepsize;
	static MObject grammar;
	//erode
	static MObject carve;
	static MObject outputPath;
	static MObject inputPath;
	//
	static MString grammarPath;

protected:
	void computeImage(std::string& carved, std::string& carvedSmoothed);
};


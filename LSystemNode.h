#define MNoVersionString 
#define MNoPluginEntry
#include <maya/MFnPlugin.h>

class LSystemNode :
	public MPxNode
{
public:
	LSystemNode();
	virtual ~LSystemNode();
	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	static void* creator();
	static MStatus initialize();

	static MTypeId nodeID;
	static MObject time;
	static MObject outputMesh;
	static MObject dfAngle;
	static MObject dfStepsize;
	static MObject grammar;

	static MString grammarPath;
protected:

	MObject createMesh(const MTime & time, const double & angle, 
		const double & stepsize, MString & grammar, 
		MObject & outData, MStatus & stat);

};



#include <maya/MPxCommand.h>
#include <maya/MFnPlugin.h>
#include <maya/MIOStream.h>
#include <maya/MArgList.h>
#include <maya/MDoubleArray.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MDGModifier.h>
#include <maya/MPlugArray.h>
#include <maya/MVector.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MStringArray.h>
#include <list>
#include <maya/MString.h>
#include <maya/MGlobal.h>
#include <maya/MSimple.h>
#include "LSystemCmd.h"
#include "LSystemNode.h"
MStatus initializePlugin( MObject obj )
{
    MStatus   status = MStatus::kSuccess;
    MFnPlugin plugin( obj, "MyPlugin", "1.0", "Any");

    // Register Command
    status = plugin.registerCommand( "LSystemCmd", LSystemCmd::creator );


	char buffer[2048];
	sprintf_s(buffer, 2048, "source \"%s/lsys.mel\";", plugin.loadPath().asChar());
	MGlobal::executeCommand(buffer, true);

    if (!status) {
        status.perror("registerCommand");
        return status;
    }

	status = plugin.registerNode("LSystemNode", LSystemNode::nodeID,
		LSystemNode::creator, LSystemNode::initialize);

	LSystemNode::grammarPath = plugin.loadPath();

	if (!status) {
		status.perror("registerNode");
		return status;
	}

    return status;
}

MStatus uninitializePlugin( MObject obj)
{
    MStatus   status = MStatus::kSuccess;
    MFnPlugin plugin( obj );

    status = plugin.deregisterCommand( "LSystemCmd" );
	MGlobal::executeCommand("deleteUI LsystemMenu");
    if (!status) {
	    status.perror("deregisterCommand");
	    return status;
    }
	status = plugin.deregisterNode(LSystemNode::nodeID);
	if (!status) {
		status.perror("deresigerNode");
		return status;
	}
    return status;
}

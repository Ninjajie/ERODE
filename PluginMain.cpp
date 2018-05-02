
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
//#include "ERODECmd.h"
#include "ERODENode.h"
MStatus initializePlugin( MObject obj )
{
    MStatus   status = MStatus::kSuccess;
    MFnPlugin plugin( obj, "MyPlugin", "1.0", "Any");

    // Register Command
   // status = plugin.registerCommand( "ERODECmd", ERODECmd::creator );

	//register the menu by a MEL script
	char buffer[2048];
	sprintf_s(buffer, 2048, "source \"%s/ERODE.mel\";", plugin.loadPath().asChar());
	MGlobal::executeCommand(buffer, true);

    //if (!status) {
     //   status.perror("registerCommand");
     //   return status;
    //}

	status = plugin.registerNode("ERODENode", ERODENode::nodeID,
		ERODENode::creator, ERODENode::initialize);

	ERODENode::grammarPath = plugin.loadPath();

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

 //   status = plugin.deregisterCommand( "ERODECmd" );
	MGlobal::executeCommand("deleteUI ERODEMenu");
 //   if (!status) {
	//    status.perror("deregisterCommand");
	//    return status;
 //   }
	status = plugin.deregisterNode(ERODENode::nodeID);
	if (!status) {
		status.perror("deresigerNode");
		return status;
	}
    return status;
}

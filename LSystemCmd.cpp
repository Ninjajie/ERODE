#include "LSystemCmd.h"
#include <maya/MGlobal.h>
#include <list>
#include <maya/MArgList.h>
#include <iostream>
#include "LSystem.h"
//#define EXPORT _declspec(dllexport) // define EXPORT for exporting as .dll, 

typedef std::pair<vec3, vec3> Branch;

LSystemCmd::LSystemCmd() : MPxCommand()
{
}

LSystemCmd::~LSystemCmd() 
{
}

MStatus LSystemCmd::doIt( const MArgList& args )
{
	// message in Maya output window
    std::cout<<"Implement Me!"<<endl;
	std::cout.flush();

	double dfStepSize, dfAngle;
	MString grammar;
	int iterationNum;
	MStatus status2;
	for (int i = 0; i < args.length(); i++)
	{
		if (MString("-stepsize") == args.asString(i, &status2) && MS::kSuccess == status2)
		{
			double tmpDouble = args.asDouble(++i, &status2);
			if (MS::kSuccess == status2)
				dfStepSize = tmpDouble;
		}
		else if (MString("-angle") == args.asString(i, &status2) && MS::kSuccess == status2)
		{
			double tmpDouble = args.asDouble(++i, &status2);
			if (MS::kSuccess == status2)
				dfAngle = tmpDouble;
		}
		else if (MString("-grammar") == args.asString(i, &status2) && MS::kSuccess == status2)
		{
			MString tmp = args.asString(++i, &status2);
			if (MS::kSuccess == status2)
				grammar = tmp;
		}
		else if (MString("-iteration") == args.asString(i, &status2) && MS::kSuccess == status2)
		{
			double tmpInt = args.asDouble(++i, &status2);
			if (MS::kSuccess == status2)
				iterationNum = int(tmpInt);
		}
	}

	LSystem lsys;
	std::vector<Branch> branches;
	lsys.loadProgram(grammar.asChar());
	lsys.setDefaultAngle(dfAngle);
	lsys.setDefaultStep(dfStepSize);
	lsys.process(iterationNum, branches);


	for (int i = 0; i < branches.size(); i++) 
	{
		vec3 startP = branches[i].first;
		vec3 endP = branches[i].second;

		MString cmd = "curve -d 1 -p ";
		for (int j = 0; j < 3; j++)
		{
			cmd += startP[j];
			cmd += " ";
		}
		cmd += "-p ";
		for (int j = 0; j < 3; j++)
		{
			cmd += endP[j];
			cmd += " ";
		}
		cmd += "-k 0 - k 1";
		MGlobal::executeCommand(cmd);
	}
    return MStatus::kSuccess;
}


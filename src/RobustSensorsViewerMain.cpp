/*******************************************************************************
 * RobustSensorsViewerMain.cpp
 * 
 * Peter Fine - DPhil '06
 ******************************************************************************/
 
using namespace std;

#include "RobustSensorsViewer.h"
#include "Properties.h"

// TEMP?
#include <iostream>
#include <sstream>

int main(int argc, char** argv) {

	Properties viewerProps;
	RobustSensorsViewer::addProperties(viewerProps);
	viewerProps.parseCommandLine(argc, argv);
		
	RobustSensorsViewer viewer(viewerProps);

	viewer.runViewer();
}

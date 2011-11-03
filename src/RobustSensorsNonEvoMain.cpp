/*******************************************************************************
 * ActiveVisionMain.cpp
 * 
 * Peter Fine - Thesis - Main Viewer Program
 * Created May '05
 * 
 ******************************************************************************/
 
using namespace std;

#include "RobustSensorsManagerNonEvo.h"
#include "NetworkFactory.h"
#include "Properties.h"

// TEMP?
#include <iostream>
#include <sstream>

int main(int argc, char** argv) {

	// Create a properties object.
	Properties simulationProperties;
	
	// Add the required properties.
	NetworkFactory::addProperties(simulationProperties);
	RobustSensorsManagerNonEvo::addProperties(simulationProperties);
	
	// And parse the command line to define these property values.
	simulationProperties.parseCommandLine(argc, argv);
				
	// Create the simulator.
	RobustSensorsManagerNonEvo myManager(simulationProperties);  
	
	// And run the simulation.
	myManager.runSimulation();

}

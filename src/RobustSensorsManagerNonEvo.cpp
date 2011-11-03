#include "RobustSensorsManagerNonEvo.h"

#include "MatlabWriter.h"
#include "NetworkFactory.h"
#include "Genotype.h"

#include <sstream>

RobustSensorsManagerNonEvo::RobustSensorsManagerNonEvo(Properties& simulationProperties) :
	mySimulationProperties(simulationProperties),
	myRand(mySimulationProperties.getInt("seed")),
	mySim(*NetworkFactory::generateNetwork(mySimulationProperties, myRand), mySimulationProperties, myRand),
	myRecorder(mySim, mySimulationProperties)
	{ }
	
double RobustSensorsManagerNonEvo::runSimulation() {
	
	//Prepare the simulation -------------------------------------------------------------------------------------
	mySim.configure();
	
	// Prepare the records and network for recording.
	myRecorder.resetRecords();

	// Run the simulation ----------------------------------------------------------------------------------------
	double timestep = mySim.myTimestep;
	double simLength = mySim.mySimLength;
	for(double t = 0.0; t < simLength; t += timestep) {
		
		// Update the simulation.
		mySim.update();
		// Record the simulation.
		myRecorder.updateRecords();
	}
	
	// Write the records to the screen (in matlab format). Start with the simulation config.
	string records = myRecorder.printRecords();
	
	// Add details which are not held by the simulation.
	myRecorder.addNonEvoSpecificData(records, mySimulationProperties);
	
	// Print the records to the screen.
	cout << records;
}


void RobustSensorsManagerNonEvo::addProperties(Properties& properties) {
	properties.addIntItem("seed");
	
	RobustSensorsSimulator::addProperties(properties);
	RobustSensorsRecorder::addProperties(properties);
}

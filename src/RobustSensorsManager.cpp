/*******************************************************************************
 * RobustSensorsManager.cpp
 * 
 * Peter Fine - EASy MSC, Thesis
 * Created March '05
 * 
 ******************************************************************************/

using namespace std;

#include "RobustSensorsManager.h"
#include "MersenneTwister.cpp"
#include "MatlabWriter.h"
#include "NeuralNetwork.h"
#include "Properties.h"

#include <numeric>
#include <iostream>

RobustSensorsManager::RobustSensorsManager(NeuralNetwork& network, Properties& properties, MTRand& random) :
	SimulationManager(properties, random), 
	mySimulation(network, properties, random),
	myRand(random),
	myNoTrials(properties.getInt("noTrials")),
	myPhaseNo(1), // Phases start at 1!
	myGenerationOfPreviousPhaseSwitch(0),
	myEndSimulationOnSuccess(properties.getBool("endSimulationOnSuccess"))
	{ } // Note that here, the population won't have been initialized yet.

bool RobustSensorsManager::shape(int generationNo) {
	// Determine whether to switch to the next phase of the shaping scheme.
	int switchRecordSize = 15;
	bool endSimulation = false;

	// Check whether the last (at least) switchRecordSize generation have shown fitness > 0.8.
	if ((myBestFitnessRecord.size() >= switchRecordSize) && // There have been enough records...
			((generationNo - myGenerationOfPreviousPhaseSwitch) > switchRecordSize)) // and the records have been replaced since the previous switch. 
	{
		bool switchNow = true;
		for (int i = myBestFitnessRecord.size() - switchRecordSize; i < myBestFitnessRecord.size(); i++) {
			if (myBestFitnessRecord[i] < 0.8) {
				switchNow = false;
			}
		}

		// Switch to the next phase.
		if (switchNow) {
			myPhaseNo++;
			myGenerationOfPreviousPhaseSwitch = generationNo;
			// If the third phase is just begining, make some configuration changes...
			if (myPhaseNo == 3) { // Readaptation. Phases start at 1!
				mySimulation.myNoLights = (mySimulation.myNoLights * 2);
				mySimulation.myChangeAngleDuringTrial = true;
			}
			// Beyond 3, run more lights (since more changes will occur).
			else if (myPhaseNo == 4) { // Continuous angles. // Phases start at 1!
				mySimulation.myNoLights = 16;
			
//				myNoTrials = myNoTrials * 4; // More complicated, so more trials.
//				mySimulation.mySensorAngle1D = false;
//				//XXXTHIS IS NOT NEEDED
			}
		}
	}
	myPhaseRecord.push_back(myPhaseNo);
	
	if(myEndSimulationOnSuccess) {
		if(myPhaseNo > 24) {
			endSimulation = true;
		}
	}
	
	return endSimulation;
}

double RobustSensorsManager::getFitness(Genotype& genotype) {

	mySimulation.reconstruct(genotype);

	vector<double> fitness;
	int totalNoTrials = 0;
	// During phase 1 or 2...
	if (myPhaseNo < 3) { // Phases start at 1!

		// Run a number of trials (phases one and two).
		totalNoTrials += myNoTrials;
		for (int trialNo = 0; trialNo < myNoTrials; trialNo++) {
			mySimulation.configure();
			mySimulation.overwriteSensorAngle(0.0);
			fitness.push_back(mySimulation.runSimulation());
		}

		// If the second phase has began, run experiments with the sensor behind the agent as well.
		if (myPhaseNo == 2) { // Phases start at 1!
			totalNoTrials += myNoTrials;
			for (int trialNo = 0; trialNo < myNoTrials; trialNo++) {
				mySimulation.configure();
				mySimulation.overwriteSensorAngle(mySimulation.myPi);
				fitness.push_back(mySimulation.runSimulation());
			}
		}
	} else if (myPhaseNo == 3) {
		// The third phase has began.
		totalNoTrials = myNoTrials * 2;

		// Sensor on front.
		for (int trialNo = 0; trialNo < myNoTrials; trialNo++) {
			mySimulation.configure();
			mySimulation.overwriteSensorAngle(0.0);

			// Set the angle to change at the middle light number, plus or minus 2 lights.
			mySimulation.generateSingleChangeAngleSchedule(
					(int)(mySimulation.myNoLights / 2), 2);
			fitness.push_back(mySimulation.runSimulation());
		}

		// Sensor on back.
		for (int trialNo = 0; trialNo < myNoTrials; trialNo++) {
			mySimulation.configure();
			mySimulation.overwriteSensorAngle(mySimulation.myPi);

			// Set the angle to change at the middle light number, plus or minus 2 lights.
			mySimulation.generateSingleChangeAngleSchedule(
					(int)(mySimulation.myNoLights / 2), 2);
			fitness.push_back(mySimulation.runSimulation());
		}
	} else { // The forth phase (many sensor angle changes) has begun.

		totalNoTrials = myNoTrials * 2;

		// Sensor on front.
		for (int trialNo = 0; trialNo < myNoTrials; trialNo++) {
			mySimulation.configure();
			mySimulation.overwriteSensorAngle(0.0);

			// Set the angle to change at the middle light number, plus or minus 2 lights.
			mySimulation.generateChangeAngleSchedule(3);
			fitness.push_back(mySimulation.runSimulation());
		}

		// Sensor on back.
		for (int trialNo = 0; trialNo < myNoTrials; trialNo++) {
			mySimulation.configure();
			mySimulation.overwriteSensorAngle(mySimulation.myPi);

			// Set the angle to change at the middle light number, plus or minus 2 lights.
			mySimulation.generateChangeAngleSchedule(3);
			fitness.push_back(mySimulation.runSimulation());
		}
	}

	return accumulate(fitness.begin(), fitness.end(), 0.0)
			/ (double)totalNoTrials;
}

bool RobustSensorsManager::checkGenomeIsValid(Genotype& genotype) {
	// Get the network to check that this genome is valid.
	return mySimulation.myNetwork.checkGenomeIsValid(genotype);
}

string RobustSensorsManager::addExtraRecords() {
	return MatlabWriter::parse(myPhaseRecord, "phaseNo");
}

void RobustSensorsManager::addProperties(Properties& properties) {
	properties.addIntItem("noTrials");
	properties.addBoolItem("endSimulationOnSuccess");

	RobustSensorsSimulator::addProperties(properties);
}

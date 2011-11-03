/*******************************************************************************
 * HomeostaticBraitenburg.cpp
 * 
 * Peter Fine - DPhil Dec '06
 ******************************************************************************/

using namespace std;

#include "HomeostaticBraitenburgNetwork.h"
#include "MatlabWriter.h"
#include "Properties.h"
#include "Genotype.h"
#include "MersenneTwister.cpp"

#include <iostream>
#include <sstream>

HomeostaticBraitenburgNetwork::HomeostaticBraitenburgNetwork(Properties& properties, MTRand& rand) : 
	NeuralNetwork(properties.getInt("noInputs"), properties.getInt("noOutputs")),
	myRand(rand),
	myXPointsLeftMotor(4),
	myYPointsLeftMotor(4),  
	myXPointsRightMotor(4),
	myYPointsRightMotor(4),
	myE1(properties.getDouble("E1")),
	myMinE(properties.getDouble("minE")),
	myMaxE(properties.getDouble("maxE")),
	myE2(properties.getDouble("E2")),
	myInitialE(properties.getDouble("initialE")),
	myTauE(properties.getDouble("tauE")),
	mySensorInfluenceOnBattery(properties.getDouble("sensorInfluenceOnBattery")),
	myMutationVariance(properties.getDouble("HomeostaticBraitenburgMutationVariance"))
{ }

void HomeostaticBraitenburgNetwork::resetNetwork() {
  
  // Reset all inputs to 0.
  for(int i = 0; i < myNoInputs; i++) {
    myInputs[i] = 0.0;
  }
  
  // Reset all outputs to 0.
  for(int i = 0; i < myNoOutputs; i++) {
    myOutputs[i] = 0.0;
  }
  
  // Initialize the transfer function points to random values.
	randomizeVector(myXPointsLeftMotor);
	randomizeVector(myYPointsLeftMotor);
	randomizeVector(myXPointsRightMotor);
	randomizeVector(myYPointsRightMotor);
	
	// Overwrite the first and last X coordinates with their preset values.
	myXPointsLeftMotor.at(0) = 0.0;
  myXPointsRightMotor.at(0) = 0.0;
  myXPointsLeftMotor.at(3) = 1.0;
  myXPointsRightMotor.at(3) = 1.0;
  
  // Reset the battery level to the initial value.
  myE = myInitialE;
}

void HomeostaticBraitenburgNetwork::updateNetwork(double timeStep) {

	// Map the input to the outputs. ---------------------------------
	
	// First sort the mapping vectors.
	sort(myXPointsLeftMotor.begin(), myXPointsLeftMotor.end());
	sort(myXPointsRightMotor.begin(), myXPointsRightMotor.end());
	
	// Should the Y points be sorted to match the X points?
		
	// Determine which value is to the immediate right of the input, for each mapping.
	int nextPointLeftMotor = -1; // -1 is a dummy values which will cause a crash.
	int nextPointRightMotor = -1;
	// This assumes xPointsLeftMotor and xPointsRightMotor are same size.
	for(int i = myXPointsLeftMotor.size() - 1; i > 0 ; i--) { // (i=0 gives 0).
		if(myInputs.at(0) < myXPointsLeftMotor.at(i)) {
			nextPointLeftMotor = i;
		}
		if(myInputs.at(0) < myXPointsRightMotor.at(i)) {
			nextPointRightMotor = i;
		}
	}
	
	// calculate the gradient at the input's location on each mapping.
	double leftGradient = (myYPointsLeftMotor.at(nextPointLeftMotor) - 
												 myYPointsLeftMotor.at(nextPointLeftMotor - 1)) /
												(myXPointsLeftMotor.at(nextPointLeftMotor) - 
												 myXPointsLeftMotor.at(nextPointLeftMotor - 1));
	double rightGradient = (myYPointsRightMotor.at(nextPointRightMotor) - 
							 					  myYPointsRightMotor.at(nextPointRightMotor - 1)) /
												 (myXPointsRightMotor.at(nextPointRightMotor) - 
												  myXPointsRightMotor.at(nextPointRightMotor - 1));
	
	// And use this to determine the outputs.
	myOutputs.at(0) = leftGradient * (myInputs.at(0) - myXPointsLeftMotor.at(nextPointLeftMotor - 1));
	myOutputs.at(0) += myYPointsLeftMotor.at(nextPointLeftMotor - 1);

	myOutputs.at(1) = rightGradient * (myInputs.at(0) - myXPointsRightMotor.at(nextPointRightMotor - 1));
	myOutputs.at(1) += myYPointsRightMotor.at(nextPointRightMotor - 1);
	
	// Update the battery. XXX Should this be clipped or transformed to an exp?
	double clippedInput = myInputs.at(0);
	if(clippedInput < 0.7) {
		clippedInput = 0.0;
	}
	//clippedInput = (6 * exp(clippedInput)) / exp(6);
	myE += timeStep * ((-myE + mySensorInfluenceOnBattery * clippedInput) / myTauE);
	
	// Mutate the mappings if E is out of its bounds.
	double mutationMultiplier = -1;
	if(myE < myMinE) {
		mutationMultiplier = min(1.0, (myMinE - myE) / (myMinE - myE1));
	}
		else if(myE > myMaxE) {
			mutationMultiplier = min(1.0, (myE - myMaxE) / (myE2 - myMaxE));
		}
		
	if(mutationMultiplier > 0.0) {
  	mutateVector(mutationMultiplier, myXPointsLeftMotor, 1, 2);
		mutateVector(mutationMultiplier, myYPointsLeftMotor, 0, 3);  
		mutateVector(mutationMultiplier, myXPointsRightMotor, 1, 2);
		mutateVector(mutationMultiplier, myYPointsRightMotor, 0, 3);
	}
}

void HomeostaticBraitenburgNetwork::addProperties(Properties& properties) {
	properties.addDoubleItem("E1");
	properties.addDoubleItem("minE");
	properties.addDoubleItem("maxE");
	properties.addDoubleItem("E2");
	properties.addDoubleItem("initialE");
	properties.addDoubleItem("tauE");
	properties.addDoubleItem("sensorInfluenceOnBattery");
	properties.addDoubleItem("HomeostaticBraitenburgMutationVariance");
}

// Utility methods. ---------------------------------------------------------------
void HomeostaticBraitenburgNetwork::randomizeVector(vector<double>& aVector) {
	for(int i = 0; i < aVector.size(); i++) {
		aVector.at(i) = myRand.randDblExc();
	}
}

void HomeostaticBraitenburgNetwork::mutateVector(double mutationMultiplier, vector<double>& aVector, 
																								 int startIndex, int endIndex) {
	for(int i = startIndex; i < endIndex; i++) {
		aVector.at(i) += mutationMultiplier * myRand.randNorm(0.0, myMutationVariance);
		while((aVector.at(i) < 0.0) || (aVector.at(i) > 1.0)) {
			if(aVector.at(i) < 0) {
				aVector.at(i) = -aVector.at(i);
			}
			if(aVector.at(i) > 1.0) {
				aVector.at(i) = 1.0 - (aVector.at(i) - 1.0);
			} 
		}
	}
}

// The Following manage the state records. -------------------------------------------

void HomeostaticBraitenburgNetwork::resetRecords() {
	
	// Clear the state containers.
	myInputRecord.clear();
	myOutputRecord.clear();
  myXPointsLeftMotorRecord.clear();
	myYPointsLeftMotorRecord.clear();  
	myXPointsRightMotorRecord.clear();
	myYPointsRightMotorRecord.clear();
	myERecord.clear();
}

void HomeostaticBraitenburgNetwork::updateRecords() {
	myInputRecord.push_back(myInputs);
	myOutputRecord.push_back(myOutputs);
	myXPointsLeftMotorRecord.push_back(myXPointsLeftMotor);
	myYPointsLeftMotorRecord.push_back(myYPointsLeftMotor);  
	myXPointsRightMotorRecord.push_back(myXPointsRightMotor);
	myYPointsRightMotorRecord.push_back(myYPointsRightMotor);
	myERecord.push_back(myE);
}

string HomeostaticBraitenburgNetwork::getRecords(bool includeState) {
	// Output the network configuration and the state since recording started.
	stringstream record;
	
	// Config
	record << MatlabWriter::parse(myE1, "E1");
	record << MatlabWriter::parse(myMinE, "minE");
	record << MatlabWriter::parse(myMaxE, "maxE");
	record << MatlabWriter::parse(myE2, "E2");
	record << MatlabWriter::parse(myInitialE, "initialE");
	record << MatlabWriter::parse(myTauE, "tauE");
	record << MatlabWriter::parse(mySensorInfluenceOnBattery, "sensorInfluenceOnBattery");
	record << MatlabWriter::parse(myMutationVariance, "HomeostaticBraitenburgMutationVariance");
		
	// State
	if(includeState) {
		record << MatlabWriter::parse(myInputRecord, "neuronInputs");
		record << MatlabWriter::parse(myOutputRecord, "neuronOutputs");
		record << MatlabWriter::parse(myXPointsLeftMotorRecord, "XPointsLeftMotor");
		record << MatlabWriter::parse(myYPointsLeftMotorRecord, "YPointsLeftMotor");
		record << MatlabWriter::parse(myXPointsRightMotorRecord, "XPointsRightMotor");
		record << MatlabWriter::parse(myYPointsRightMotorRecord, "YPointsRightMotor");
		record << MatlabWriter::parse(myERecord, "E");
	}
	
	return record.str();
}

/*******************************************************************************
 * RobustSensorsSimulator.cpp
 * 
 * Peter Fine - EASy MSC, Thesis
 * Created March '05
 * 
 ******************************************************************************/

using namespace std;

#include "RobustSensorsSimulator.h"
#include "Genotype.h"
#include "MersenneTwister.cpp"
#include "Properties.h"

RobustSensorsSimulator::RobustSensorsSimulator(NeuralNetwork& network, Properties& properties, 
												 MTRand& random)
	:	myNetwork(network),
		myProperties(properties),
		myRand(random),
		myNoLights(properties.getInt("noLights")),
		myOverwriteLightAngle(false), // Follow the pre-coded angles in resetLight.
		myFitnessDuration(properties.getDouble("fitnessDuration")),
		myTimestep(properties.getDouble("timestep")),
		myTime(0.0),
		myRadius(properties.getDouble("agentRadius")),
		myLightDurationMin(properties.getDouble("lightDurationMin")),
		myLightDurationMax(properties.getDouble("lightDurationMax")),
		myLightDistanceMin(properties.getDouble("lightDistanceMin")),
		myLightDistanceMax(properties.getDouble("lightDistanceMax")),
		myPi(3.14159265),
		myChangeAngleDuringTrial(false),
		myExperimentHasEnded(false),
		mySensorAngle1D(properties.getBool("sensorAngle1D"))
{ }

void RobustSensorsSimulator::configure() {
	
	myUnNormalizedFitness = 0.0;
	myTotalFitnessTime = 0.0;
	myExperimentHasEnded = false;
	
	myNetwork.resetNetwork();
	myTime = 0.0;
	// Reset the agent's position and generate a light position.
	myAgentX = 0.0;
	myAgentY = 0.0;
	myAgentHeading = 0.0;
	myLightNo = 0;
	resetLight(false); // The first light doesn't contribute to fitness.
	
	// Set an inital sensor angle of 0.5 pi (which could be identified as coming from this line).
	mySensor1Angle = myPi / 2;
}

void RobustSensorsSimulator::overwriteSensorAngle(double angle1) {
	mySensor1Angle = angle1;
}

// Note that the below is only called if the simulation is of a genetic nature.
void RobustSensorsSimulator::reconstruct(Genotype& genotype) {
	myNetwork.reconstructNetwork(genotype);
}

double RobustSensorsSimulator::runSimulation() {

	// Run the simulation.
	while(!myExperimentHasEnded) {
	
		update();
	}
	
	// Normalize the fitness and return.
	if(myTotalFitnessTime > 0.0) { return myUnNormalizedFitness / myTotalFitnessTime; }
	else { return 0.0; } 
}

// UPDATE ---------------------------------------------------------------------------------------------------------------
void RobustSensorsSimulator::update() {

	// Determine whether to move to the next light and sensor angle -----------------------------------------------------
	if(myTimeUntilNextLight <= 0.0) {
		
		// Check to see if the experiment has ended. If so, return.
		if(myLightNo == (myNoLights - 1)) {
			myExperimentHasEnded = true;
			return;
		}
		
		myLightNo++;
		
		// Now determine if the angle should be changed or not.
		bool angleWasChanged = changeAngleIfRequired();
		bool lightContributesToFitness = !angleWasChanged; // If the angle was changed, the light won't contribute.
		
		// Reset the light. Note that reset is also called in configure();
		resetLight(lightContributesToFitness);
	}
	myTimeUntilNextLight -= myTimestep;

	// Calculate Sensor Inputs -------------------------------------------------------------------------
	// Calculate the sensor locations.
	double agentDistSq   = pow(myLightX - myAgentX, 2) + pow(myLightY - myAgentY, 2);
	double agentDist = sqrt(agentDistSq);

	mySensor1X = myAgentX + myRadius * cos(myAgentHeading + mySensor1Angle);
	mySensor1Y = myAgentY + myRadius * sin(myAgentHeading + mySensor1Angle);
	double sensor1DistSq = pow(myLightX - mySensor1X, 2) + pow(myLightY - mySensor1Y, 2); 
	double sensor1Dist = sqrt(sensor1DistSq);
	
			
	// Calculate whether the light impinges on the sensor 1.
	mySensor1Input = 0.0;
	if((pow(myRadius, 2) + sensor1DistSq) / agentDistSq <= 1) {
		// Calculate the linear sensor input.
		if(sensor1Dist < myLightDistanceMax * 1.5) {
			mySensor1Input = (1 - sensor1Dist / (myLightDistanceMax * 1.5));
		}
	}
	
	myNetwork.setInput(0, mySensor1Input); 
	
	// Update the network and calculate the motor outputs ----------------------------------------------------------------
	
	myNetwork.updateNetwork(myTimestep);

	double moveV = (myNetwork.getOutput(0) + myNetwork.getOutput(1)) / 2;
	double turnV = (myNetwork.getOutput(0) - myNetwork.getOutput(1)) / (2 * myRadius);
	
	myAgentX += myTimestep * moveV * cos(myAgentHeading);
	myAgentY += myTimestep * moveV * sin(myAgentHeading);
	myAgentHeading += myTimestep * turnV;
	
	// Only add this to the fitness value if...
	if(myLightContributesToFitness && // This light is not part of the initial phase (or after a change of angle). 
		 (myTimeUntilNextLight <= myFitnessDuration)) { // The time is within the fitness contributing phase of the current light
		double currentFitness = 0.0;
		if(agentDist < myLightDistanceMax) {
			currentFitness = 1.0 - (agentDist / myLightDistanceMax);
		}
		
		myUnNormalizedFitness += currentFitness * myTimestep;
		myTotalFitnessTime += myTimestep;
	}
	
	myTime += myTimestep;
	
}
// RESET LIGHT ------------------------------------------------------------------------------------------------------------------
void RobustSensorsSimulator::resetLight(bool contributesToFitness) {
	
	myLightContributesToFitness = contributesToFitness;
	
	myTimeUntilNextLight = myRand.rand() * (myLightDurationMax - myLightDurationMin) + myLightDurationMin;
	//myTimeUntilNextLight = 50.0;
	
	double lightAngle = myRand.rand() * 2.0 * myPi;
	double lightDistance = myRand.rand() * (myLightDistanceMax - myLightDistanceMin) + myLightDistanceMin;
	
	/* Overwrite the light angles, if requested. Only for the first few lights.
	if(myOverwriteLightAngle) {
		lightDistance = 0.5 * (myLightDistanceMax - myLightDistanceMin) + myLightDistanceMin;
		switch(myLightNo) {
			case 0 : lightAngle = myPi; break;
			case 1 : lightAngle = myPi; break;
			case 2 : lightAngle = 0.0; break;
			case 3 : lightAngle = 0.0; break;
			case 4 : lightAngle = myPi; break;
		}
	}*/

	myLightX = myAgentX + cos(lightAngle) * lightDistance;
	myLightY = myAgentY + sin(lightAngle) * lightDistance;

}

// CHANGE ANGLE ------------------------------------------------------------------------------------------------------------------
bool RobustSensorsSimulator::changeAngleIfRequired() {
		
	bool angleWasChanged = false;
	if(myChangeAngleDuringTrial) {
		if(count(myChangeAngleSchedule.begin(), myChangeAngleSchedule.end(), myLightNo) > 0) {
			// Change the sensor angle.
			angleWasChanged = true;
				
		if(mySensorAngle1D) {
			// Flip the sensor angle.
			if(mySensor1Angle < (myPi / 2)) { overwriteSensorAngle(myPi); }
			else {overwriteSensorAngle(0.0); }
		}
			else { // Sensors not 1D
				// Select a random angle.
				overwriteSensorAngle(myRand.rand() * 2 * myPi);
			}
		}
	}
	
	return angleWasChanged;
}

// Generate change angles -----------------------------------------------------------------------------------------------------
void RobustSensorsSimulator::generateChangeAngleSchedule(int noChanges) {
	myChangeAngleSchedule.clear();
	for(int i = 0; i < noChanges; i++) {
		int nextChangeLightNo;
		do {
			nextChangeLightNo = myRand.randInt(myNoLights - 1);
		}	while((count(myChangeAngleSchedule.begin(), myChangeAngleSchedule.end(), nextChangeLightNo) != 0) &&
		        (nextChangeLightNo != 0));
		
		myChangeAngleSchedule.push_back(nextChangeLightNo);
	}
	
	sort(myChangeAngleSchedule.begin(), myChangeAngleSchedule.end());
}

void RobustSensorsSimulator::setChangeAngleSchedule(vector<int>& aChangeAngleSchedule) {
	myChangeAngleSchedule = aChangeAngleSchedule;
}

void RobustSensorsSimulator::generateSingleChangeAngleSchedule(int meanLightNo, int maxVariationFromMean) {
	myChangeAngleSchedule.clear();
	myChangeAngleSchedule.push_back((int)(myNoLights / 2) + (myRand.randInt(4) - 2));
}

void RobustSensorsSimulator::addProperties(Properties& properties) {
	properties.addDoubleItem("timestep");
	properties.addIntItem("noLights");
	properties.addDoubleItem("agentRadius");
	properties.addDoubleItem("lightDurationMin");
	properties.addDoubleItem("lightDurationMax");
	properties.addDoubleItem("lightDistanceMin");
	properties.addDoubleItem("lightDistanceMax");
	// The fitnessDuration is the time before each light change during which fitness is recorded.
	properties.addDoubleItem("fitnessDuration");
	properties.addDoubleItem("angleDuration");
	properties.addBoolItem("sensorAngle1D");
	//properties.addDoubleItem("xxxAngle");
}

/*******************************************************************************
 * RobustSensorsRecorder.cpp
 * 
 * Peter Fine - DPhil - Holds records of the state and properties of a Simulation.
 * Created Dec '06
 * 
 ******************************************************************************/

#include "RobustSensorsRecorder.h"
#include "MatlabWriter.h"
#include "Properties.h"
#include "RobustSensorsSimulator.h"
#include <sstream>

RobustSensorsRecorder::RobustSensorsRecorder(RobustSensorsSimulator& sim, 
																						 Properties& recorderProperties) : 
	mySim(sim), 
	myFrequency(recorderProperties.getInt("recordFrequency")), 
	myCallsTillNextUpdate(0),
	myRecordSimulationData(recorderProperties.getBool("recordSimulationData"))
	{ 
		MatlabWriter::myExtraPrefix = recorderProperties.getString("extraPrefix"); 
	};
		

void RobustSensorsRecorder::resetRecords() {
	myRecAgentX.clear();
	myRecAgentY.clear();
	myRecLightX.clear();
	myRecLightY.clear();
	myRecLightNo.clear();
	myRecSensor1X.clear();
	myRecSensor1Y.clear();
	myRecAgentHeading.clear();
	myRecTime.clear();
	myRecTimeUntilNextLight.clear();
	myRecUnNormalizedFitness.clear();
	myRecSensor1Input.clear();
	myRecSensor1Angle.clear();
	myRecLightContributesToFitness.clear();
	
	// Tell the network to reset its records.
	mySim.myNetwork.resetNetwork();
}

void RobustSensorsRecorder::updateRecords() {
	if(!myRecordSimulationData) {
		return;
	}
	
	myCallsTillNextUpdate--;
	
	if(myCallsTillNextUpdate <= 0) {
		myCallsTillNextUpdate = myFrequency;
		
		myRecAgentX.push_back(mySim.myAgentX);
		myRecAgentY.push_back(mySim.myAgentY);
		myRecLightX.push_back(mySim.myLightX);
		myRecLightY.push_back(mySim.myLightY);
		myRecLightNo.push_back(mySim.myLightNo);
		myRecSensor1X.push_back(mySim.mySensor1X);
		myRecSensor1Y.push_back(mySim.mySensor1Y);
		myRecAgentHeading.push_back(mySim.myAgentHeading);
		myRecTime.push_back(mySim.myTime);
		myRecTimeUntilNextLight.push_back(mySim.myTimeUntilNextLight);
		myRecUnNormalizedFitness.push_back(mySim.myUnNormalizedFitness);
		myRecSensor1Input.push_back(mySim.mySensor1Input);
		myRecSensor1Angle.push_back(mySim.mySensor1Angle);
		myRecLightContributesToFitness.push_back(mySim.myLightContributesToFitness);

		// Tell the network to update it's records.
		mySim.myNetwork.updateRecords();
	}
}

string RobustSensorsRecorder::printRecords() {
	// Generate a Matlab representation of all records.
	stringstream records;
	
	records << MatlabWriter::parse(mySim.myTime, "simLength");
	records << MatlabWriter::parse(mySim.myNoLights, "noLights");
	records << MatlabWriter::parse(mySim.myFitnessDuration, "fitnessDuration");
	records << MatlabWriter::parse(mySim.myTimestep, "timestep");
	records << MatlabWriter::parse(mySim.myRadius, "radius");
	records << MatlabWriter::parse(mySim.myLightDurationMin, "lightDurationMin");
	records << MatlabWriter::parse(mySim.myLightDurationMax, "lightDurationMax");
	records << MatlabWriter::parse(mySim.myLightDistanceMin, "lightDistanceMin");
	records << MatlabWriter::parse(mySim.myLightDistanceMax, "lightDistanceMax");
	records << MatlabWriter::parse(mySim.myProperties.getInt("noInterneurons"), "noInterneurons");
	records << MatlabWriter::parse(mySim.myChangeAngleDuringTrial, "changeAngleDuringTrial");
	records << MatlabWriter::parse(mySim.mySensor1Angle, "finalSensor1Angle");
	records << MatlabWriter::parse(mySim.myAgentX, "finalAgentXPos");
	records << MatlabWriter::parse(mySim.myChangeAngleSchedule, "changeAngleSchedule");
	
	records << mySim.myNetwork.getRecords(myRecordSimulationData);
	
	// Then write the simulation's state over time, if requested.
	if(myRecordSimulationData) {
		records << MatlabWriter::parse(myRecAgentX, "agentX");
		records << MatlabWriter::parse(myRecAgentY, "agentY");
		records << MatlabWriter::parse(myRecLightX, "lightX");
		records << MatlabWriter::parse(myRecLightY, "lightY");
		records << MatlabWriter::parse(myRecLightNo, "lightNo");
		records << MatlabWriter::parse(myRecSensor1X, "sensor1X");
		records << MatlabWriter::parse(myRecSensor1Y, "sensor1Y");
		records << MatlabWriter::parse(myRecAgentHeading, "agentHeading");
		records << MatlabWriter::parse(myRecTime, "time");
		records << MatlabWriter::parse(myRecTimeUntilNextLight, "timeUntilNextLight");
		records << MatlabWriter::parse(myRecSensor1Angle, "sensor1Angle");
		records << MatlabWriter::parse(myRecLightContributesToFitness, "lightContributesToFitntess");

		records << MatlabWriter::parse(myRecUnNormalizedFitness, "cumulativeUnNormalizedFitness");
		records << MatlabWriter::parse(myRecSensor1Input, "sensor1Input");		
	}
	
	return records.str();
}

void RobustSensorsRecorder::addViewerSpecificData(string& records, Properties& viewerProperties, double fitness, vector<double> lightChangeTimes) 
{
	records.append(MatlabWriter::parse(mySim.myProperties.getInt("seed"), "EvolutionSeed"));
	records.append(MatlabWriter::parse(viewerProperties.getInt("viewer_seed"), "ViewerSeed"));
	records.append(MatlabWriter::parse(fitness, "fitness"));
	records.append(MatlabWriter::parse(lightChangeTimes, "lightChangeTimes"));
}

void RobustSensorsRecorder::addNonEvoSpecificData(string& records, Properties& simulationProperties) {
	records.append( MatlabWriter::parse(mySim.myProperties.getInt("seed"), "seed"));
}

void RobustSensorsRecorder::addProperties(Properties& properties) {
	properties.addBoolItem("recordSimulationData");
	properties.addIntItem("recordFrequency");
	properties.addStringItem("extraPrefix");
}


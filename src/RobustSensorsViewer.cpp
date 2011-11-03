#include "RobustSensorsViewer.h"

#include "MatlabWriter.h"
#include "NetworkFactory.h"
#include "Genotype.h"

#include <sstream>

RobustSensorsViewer::RobustSensorsViewer(Properties& viewerProps) :
  myViewerProperties(viewerProps),
	mySimulationProperties(viewerProps.getString("viewer_dataPath")),
	myRand(viewerProps.getInt("viewer_seed"))
{
	// Determine if the number of lights should be overwritten.
	if(myViewerProperties.getBool("viewerOverwrites_noLights")) {
		mySimulationProperties.addIntItem("noLights", myViewerProperties.getInt("viewer_noLights"));
	}

	// Determine if the timestep should be overwritten.
	if(myViewerProperties.getBool("viewerOverwrites_timestep")) {
		mySimulationProperties.addDoubleItem("timestep", myViewerProperties.getDouble("viewer_timestep"));
	}
	
	// Switch the sigmoidal function to exp rather than a lookup table.
	mySimulationProperties.addBoolItem("useLookup", false);

	// Construct the simulation and recorder.
	mySim = new RobustSensorsSimulator(*NetworkFactory::generateNetwork(mySimulationProperties, myRand),
																		 mySimulationProperties,
																		 myRand);
	myRecorder = new RobustSensorsRecorder(*mySim, viewerProps);
	
	
	// Determine if the changing of the angle during the trial should be overwritten.
	if(myViewerProperties.getBool("viewerOverwrites_changeAngleDuringTrial")) {
		mySim->myChangeAngleDuringTrial = myViewerProperties.getBool("viewer_changeAngleDuringTrial");
		
		if(mySim->myChangeAngleDuringTrial) {
			
			// Determine whether an angle schedule is supplied.
			if(myViewerProperties.getBool("viewerOverwrites_changeAngleSchedule")) {
				// A schedule is supplied (delimited by "-". Convert and use it.
				vector<string> stringSchedule = 
					tokenize(myViewerProperties.getString("viewer_changeAngleSchedule"), "-");
				vector<int> schedule = convertStringVecToIntVec(stringSchedule);
				mySim->setChangeAngleSchedule(schedule);
			}
				else {
					// Generate a random schedule for the given number of changes.
					mySim->generateChangeAngleSchedule(myViewerProperties.getInt("viewer_noAngleChanges"));
				}
		}
			else {
				mySim->generateChangeAngleSchedule(0);
			}
	}
}
	
void RobustSensorsViewer::runViewer() {
		
	// Create a genotype using the genes specified in the simulation properties.
	Genotype theGenotype(mySimulationProperties);
	
	mySim->myNetwork.checkGenomeIsValid(theGenotype);
	
	// Reconstruct the simulation using this genotype.
	mySim->reconstruct(theGenotype);
	
	// Run the simulation.
	runSimulation();
}
	
double RobustSensorsViewer::runSimulation() {
	
	// Configure a simulation.
	mySim->configure();
	
	// Prepare the records and network for recording.
	myRecorder->resetRecords();
	
	mySim->overwriteSensorAngle(myViewerProperties.getDouble("viewer_sensor1Angle"));
	
	// Run the simulation.
	vector<double> lightChangeTimes;
	double prevLightX = mySim->myLightX; // Ignore the first light, which changes at 0!
	while(!mySim->myExperimentHasEnded) {
		mySim->update(); // Update the simulation.
		myRecorder->updateRecords(); // Update the recorder.

		// Check to see if the light has changed.
		if(mySim->myLightX != prevLightX) {
			lightChangeTimes.push_back(mySim->myTime);
			prevLightX = mySim->myLightX;
		}
	}
	
	// Calculate the fitness...
	double fitness = 0.0;
	if(mySim->myTotalFitnessTime > 0.0) {	fitness = mySim->myUnNormalizedFitness / mySim->myTotalFitnessTime; }

	if(myViewerProperties.getBool("viewer_matlabFormat")) {

		// Write the records to the screen (in matlab format). Start with the simulation config.
		string records = myRecorder->printRecords();
	
		// Add the evolution and viewer seeds, which are not part of the simulation and so handled by myRecorder	
		myRecorder->addViewerSpecificData(records, myViewerProperties, fitness, lightChangeTimes);
	
		// Print the records to the screen.
		cout << records;
	}
		else {
			cout << fitness << endl;
		}

	return fitness;
}


void RobustSensorsViewer::addProperties(Properties& properties) {
	properties.addStringItem("viewer_dataPath");
	properties.addDoubleItem("viewer_sensor1Angle");
	properties.addIntItem("viewer_seed");
	properties.addBoolItem("viewerOverwrites_noLights");
	properties.addIntItem("viewer_noLights");
	properties.addBoolItem("viewerOverwrites_changeAngleDuringTrial");
	properties.addBoolItem("viewer_changeAngleDuringTrial");
	properties.addIntItem("viewer_noAngleChanges");
	properties.addBoolItem("viewerOverwrites_changeAngleSchedule");
	properties.addStringItem("viewer_changeAngleSchedule");	
	properties.addBoolItem("viewerOverwrites_timestep");
	properties.addDoubleItem("viewer_timestep");
	properties.addBoolItem("viewer_matlabFormat");
	//properties.addDoubleItem("viewer_xxxAngle");

		
	RobustSensorsRecorder::addProperties(properties);
}

// ROLL THIS INTO SOME KIND OF MISC UTILS CLASS LATER.
vector<string> RobustSensorsViewer::tokenize(const string& str, const string& delimiters)
{
	vector<string> tokens;
  // Skip delimiters at beginning.
  string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  string::size_type pos     = str.find_first_of(delimiters, lastPos);

  while (string::npos != pos || string::npos != lastPos)
  {
    // Found a token, add it to the vector.
    tokens.push_back(str.substr(lastPos, pos - lastPos));
    // Skip delimiters.  Note the "not_of"
    lastPos = str.find_first_not_of(delimiters, pos);
    // Find next "non-delimiter"
    pos = str.find_first_of(delimiters, lastPos);
  }
  return tokens;
}

// ROLL THIS INTO SOME KIND OF MISC UTILS CLASS LATER.
vector<int> RobustSensorsViewer::convertStringVecToIntVec(vector<string>& strings) {
	vector<int> results;
	for(int i = 0; i < strings.size(); i++) {
		int val;
		if(EOF == sscanf(strings[i].c_str(), "%d", &val)) {
			cout << "ERROR IN convertStringVecToIntVec, quitting" << endl;
			exit(1);
		}
		results.push_back(val);
	}
	return results;
}

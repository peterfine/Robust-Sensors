/*******************************************************************************
 * SimulationManager.cpp
 * 
 * Peter Fine - EASy MSC, Adaptive Systems Coursework
 * Created March '05
 * 
 ******************************************************************************/

#include "SimulationManager.h"
#include "Genotype.h"
#include "MersenneTwister.cpp"
#include "Properties.h"
#include "MatlabWriter.h"

#include <sstream>
#include <iostream>
#include <sstream>
#include <utility>

using namespace std;

SimulationManager::SimulationManager(Properties& properties, MTRand& random) :
	myProperties(properties), myRand(random), myGA(myProperties, random, *this),
			myRecordRecorder(myProperties.getString("outputPath")
					+ myProperties.getString("recordFileName"), true),
			myDataRecorder(myProperties.getString("outputPath")
					+ myProperties.getString("dataFileName"), true),
			myDisplayResults(myProperties.getBool("displayResults")),
			myBestFitness(0.0) { } // Note that here, the population won't have been initialized yet.

void SimulationManager::runSimulation() {

	int noAgents = myGA.getPopulationSize();
	int noGenerations = myProperties.getInt("noGenerations");

	// For each generation...
	for (int gen = 0; gen < noGenerations; gen++) {

		// Run the simulations ---------------------------------------------------------------------------------------

		// Set each agent's evaluated flag to false.
		for (int agentNo = 0; agentNo < noAgents; agentNo++) {
			myGA.getGenotype(agentNo).myBeenEvaluated = false;
		}

		// Run popSize tournaments.
		for (int tournamentRound = 0; tournamentRound < noAgents; tournamentRound++) {

			// Get the indicies of the pair of agents taking part in this tournament.
			pair<int, int> agentPair;
			agentPair = myGA.getAgentPair();

			// Calculate the fitness of these agents, if they've not already been evaluated.
			Genotype& firstAgent = myGA.getGenotype(agentPair.first);
			Genotype& secondAgent = myGA.getGenotype(agentPair.second);

			if (firstAgent.myBeenEvaluated == false) {
				firstAgent.myFitness = getFitness(firstAgent);
				firstAgent.myBeenEvaluated = true;
			}

			if (secondAgent.myBeenEvaluated == false) {
				secondAgent.myFitness = getFitness(secondAgent);
				secondAgent.myBeenEvaluated = true;
			}

			// Run the tournament between these two agents.
			myGA.runTournament(agentPair);
		}

		// Apply the shaping scheme.
		// If it returns true, the simulation should end after writing the next set of records.--------------------------
		bool simulationHasEnded = false;
		if(shape(gen)) {
			// End the simulation.
			simulationHasEnded = true;
		}
		
		// Calculate Performance ---------------------------------------------------------------------------------------

		/* Calculate the best and the average fitnesses out of those which were evaluated
		 * during the previous 'generation' of the steady state GA.
		 */
		double bestFitness = -1;
		int bestFitnessIndex = -1;
		double averageFitness = 0.0;
		int noEvaluated = 0;
		for (int agentNo = 0; agentNo < noAgents; agentNo++) {

			Genotype& theAgent = myGA.getGenotype(agentNo);

			if (theAgent.myBeenEvaluated) {
				if (theAgent.myFitness > bestFitness) {
					bestFitness = theAgent.myFitness;
					bestFitnessIndex = agentNo;
				}

				averageFitness += theAgent.myFitness;
				noEvaluated++;
			}
		}
		averageFitness /= noAgents;

		// Update the records ---------------------------------------------------------------------------------------

		// Record the result of this generation.
		myBestFitnessRecord.push_back(bestFitness);
		myAverageFitnessRecord.push_back(averageFitness);

		// Update the recorder with this latest data.
		myRecordRecorder.updateRecorder(MatlabWriter::parse(
				myBestFitnessRecord, "BestFitness"));
		myRecordRecorder.updateRecorder(MatlabWriter::parse(
				myAverageFitnessRecord, "AverageFitness"));
		myRecordRecorder.updateRecorder(addExtraRecords());

		// Update the properties object with the best genotype.
		Genotype& bestAgent = myGA.getGenotype(bestFitnessIndex);

		bestAgent.write(myProperties);
		myDataRecorder.updateRecorder(myProperties);

		// Write the Records to their files.
		myRecordRecorder.writeRecord();
		myDataRecorder.writeRecord();

		// Backup this generation to a seperate folder.
		if (myProperties.getBool("keepOldGenerationData")) {

			stringstream seedSStream;
			seedSStream << myProperties.getInt("seed");
			if (gen == 0) {
				// Make a directory to store the old data.
				string cmd = "mkdir " + myProperties.getString("outputPath")
						+ seedSStream.str();
				system(cmd.c_str());
			}

			stringstream generationNoSStream;
			generationNoSStream << (gen + 1);
			string cmd = "cp " + myProperties.getString("outputPath")
					+ myProperties.getString("dataFileName") + " "
					+ myProperties.getString("outputPath") + seedSStream.str()
					+ "/" + generationNoSStream.str() + "_"
					+ myProperties.getString("dataFileName");
			system(cmd.c_str());
		}

		// Display results to the screen, if requested.
		if (myDisplayResults) {
			stringstream toDisplay;
			toDisplay << endl << "GENERATION " << gen+1 << " COMPLETE" << endl;

			toDisplay << " BEST AGENT FITNESS : " << bestFitness << endl;
			toDisplay << " AVERAGE FITNESS    : " << averageFitness << endl;
			cout << toDisplay.str();
		}

		// Inform the GA of the current best fitness score (for use in calculating possible variable mutation rates).
		myGA.updateMutationVariance(bestFitness);
		
		// Quit the simulation if it has ended.
		if(simulationHasEnded) exit(0);
	}
}

void SimulationManager::addProperties(Properties& properties) {
	// Define the random seed.
	properties.addIntItem("seed");
	// Define the properties used by the Recorder.
	properties.addStringItem("outputPath");
	properties.addStringItem("dataFileName");
	properties.addStringItem("recordFileName");
	properties.addBoolItem("keepOldGenerationData"); // keep the best agent record from previous generation.
	// Define the properties used by the SimulationManager.
	properties.addIntItem("noGenerations");
	properties.addBoolItem("displayResults");
}

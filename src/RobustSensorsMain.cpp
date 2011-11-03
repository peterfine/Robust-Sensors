/*******************************************************************************
 * XMain.cpp
 * 
 * Peter Fine - Thesis - Main Program
 * Created May '05
 * 
 ******************************************************************************/
 
using namespace std;

#include "RobustSensorsManager.h"
#include "MersenneTwister.cpp"
#include "NeuralNetwork.h"
#include "NetworkFactory.h"
#include "Properties.h"
#include "GeneticAlgorithm.h"

// TEMP?
#include <iostream>

int main(int argc, char** argv) {
	
	// Create a new Properties object.
	Properties properties;

	NetworkFactory::addProperties(properties);
	SimulationManager::addProperties(properties);
	RobustSensorsManager::addProperties(properties);
	GeneticAlgorithm::addProperties(properties);
	
	// And parse the command line to define these property values.
	properties.parseCommandLine(argc, argv);

	// Create the random number generator.
	MTRand random(properties.getInt("seed"));

	// Construct the specified network.
	NeuralNetwork* theNetwork = NetworkFactory::generateNetwork(properties, random);

	// Determine the number of genes required.
	properties.addIntItem("noGenes", theNetwork->getNoGenes());

	// Create the simulator and then initialize the random population.
	RobustSensorsManager myManager(*theNetwork, properties, random);
	
	myManager.myGA.initializePopulation();
	
	// And run the simulation.
	myManager.runSimulation();

}

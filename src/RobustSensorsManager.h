/*******************************************************************************
 * RobustSensorsManager.h
 * 
 * Peter Fine - EASy MSC, Thesis - Active Vision experiment manager.
 * Created March '05
 * 
 ******************************************************************************/

#ifndef RobustSensorsManager_h
#define RobustSensorsManager_h

#include "RobustSensorsSimulator.h"
#include "SimulationManager.h"

#include <vector>

class MTRand;
class NeuralNetwork;
class Properties;

/**
 * Sets up the simulation and, using a GeneticAlgorithm, controls the 
 * experiments that is runs.
 */
class RobustSensorsManager : public SimulationManager {
private:
	/**
	 * The simulation that this manager controls.
	 */
	RobustSensorsSimulator mySimulation;

	/**
	 * The Random number generator.
	 */
	MTRand& myRand;

	int myNoTrials;

	int myPhaseNo;
	int myGenerationOfPreviousPhaseSwitch;
	vector<int> myPhaseRecord;
	bool myEndSimulationOnSuccess;

public:
	/**
	 * Create a Simulation Manager with a given control system.
	 * @param network The NeuralNetwork controller for the agent.
	 * @param properties The Properties which configure this simulation.
	 * @param random The random number generator.
	 */
	RobustSensorsManager(NeuralNetwork& network, Properties& properties,
			MTRand& random);

	/**
	 * Configures and runs a simulation, and returns its fitness.
	 * @param genotype The genotype to construct the controller from.
	 */
	double getFitness(Genotype& genotype);

	/**
	 * Applies a shaping scheme, called at the start of each generation.
	 * @param generationNo The current generation number, for possible use by
	 * a shaping scheme, perhaps.
	 * @return Whether to quit the simulation.
	 */
	bool shape(int generationNo);

	/**
	 * Adds a definition of the properties required by this class to a 
	 * Properties object.
	 */
	static void addProperties(Properties& properties);

	/**
	 * Add any extra data to the records, should be in the form of a matlab string.
	 */
	string addExtraRecords();
    
    /*
     * Ensure that this genome conforms to any of the neural network's constraints.
     */
    bool checkGenomeIsValid(Genotype& genotype);
};

#endif // RobustSensorsManager_h

/*******************************************************************************
 * SimulationManager.h
 * 
 * Peter Fine - EASy MSC, Adaptive Systems Coursework
 * Created March '05
 * 
 ******************************************************************************/

#ifndef SimulationManager_h
#define SimulationManager_h

#include "GeneticAlgorithm.h"
#include "Recorder.h"
#include "CheckValidGenomeImplementer.h"
#include <vector>

class Genotype;
class MTRand;
class Properties;

/**
 * Sets up the simulation and, using a GeneticAlgorithm, controls the 
 * experiments that is runs.
 */
class SimulationManager : public CheckValidGenomeImplementer {

protected:

	/**
	 * The Properites object which configures this simulation.
	 */
	Properties& myProperties;

	/**
	 * The random number generator.
	 */
	MTRand& myRand;

public:
	/**
	 * The GeneticAlgorithm.
	 */
	GeneticAlgorithm myGA;
	
protected:
	/**
	 * The recorder object which records the simulation records.
	 */
	Recorder myRecordRecorder;

	/**
	 * The recorder object which records the simulation data.
	 */
	Recorder myDataRecorder;

	/**
	 * Whether to display the results of each generation to the screen or not.
	 */
	bool myDisplayResults;

	/**
	 * The most recent best fitness, potentially for use in shaping schemes.
	 */
	double myBestFitness;

	vector<double> myBestFitnessRecord;
	vector<double> myAverageFitnessRecord;

public:
	/**
	 * Create a Simulation Manager with a given control system.
	 * @param properties The Properties which configure this simulation.
	 * @param random The random number generator.
	 */
	SimulationManager(Properties& properties, MTRand& random);

	/**
	 * Run the simulation.
	 */
	void runSimulation();

	/**
	 * Configures and runs a simulation, and returns its fitness.
	 * @param genotype The genotype to construct the controller from.
	 */
	virtual double getFitness(Genotype& genotype) = 0;
	
	/**
	 * Applies a shaping scheme, called at the start of each generation.
	 * @param generationNo The current generation number, for possible use by
	 * a shaping scheme, perhaps.
	 * @returns Whether the simulation has ended or not.
	 */
	virtual bool shape(int generationNo) = 0;	

	/**
	 * Add any extra data to the records, should be in the form of a matlab string.
	 */
	virtual string addExtraRecords() {
		return "";
	}

	/**
	 * Adds a definition of the properties required by this class to a 
	 * Properties object. This includes those used by the recorder.
	 */
	static void addProperties(Properties& properties);

	/**
	 * Call a genome is valid for this simulation.
	 */
	virtual bool checkGenomeIsValid(Genotype& genotype) = 0;
};

#endif

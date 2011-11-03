/*******************************************************************************
 * GeneticAlgorithm.h
 * 
 * Peter Fine - EASy MSC, Adaptive Systems Coursework
 * Created March '05
 * 
 ******************************************************************************/

#ifndef GeneticAlgorithm_h
#define GeneticAlgorithm_h

// Forward Declarations
class MTRand;
class CheckValidGenomeImplementer;

#include "Genotype.h"

#include <utility>

/**
 * A generic genetic algorithm.
 */
class GeneticAlgorithm {
private:

	/**
	 * The array of genotypes used by the GA.
	 */
	std::vector<Genotype> myPopulation;

	/**
	 * The random number generator used by the GA.
	 */
	MTRand& myRand;

	/**
	 * The checker which ensures a genotype is acceptable to its user.
	 */
	CheckValidGenomeImplementer& myGenomeChecker;

	/**
	 * The size of the population.
	 */
	int myPopSize;

	/**
	 * The number of genes.
	 */
	int myNoGenes;

	/**
	 * The variance of each gene's gaussian mutation.
	 */
	double myMutationVariance;

	/**
	 * Whether to replace a random member of the population. Otherwise, the loser is replaced.
	 */
	bool myRandomReplacement;

	/**
	 * Whether to use demes or not in selecting competing agent pairs.
	 */
	bool myUseDemes;

	/**
	 * The deme width.
	 */
	int myDemeWidth;

	bool myVariableMutationVariance;
	double myVariableMutationVarianceMin;
	double myVariableMutationVarianceMax;
	double myVariableMutationVarianceMinFitnessThreshold;
	double myVariableMutationVarianceMaxFitnessThreshold;

	/**
	 * Muate a genotype.
	 * @param genotype The genotype to mutate.
	 */
	void mutate(Genotype& genotype);

public:

	/**
	 * Create a genetic algorithm.
	 * @param theProperties A database of properties which contains various configuration parameters.
	 * @param random A Random number generator.
	 * @param genomeChecker A class implementing the function checkGenomeIsValid, which ensures initialized/mutated genomes are
	 * permitted by whatever criterion their user requires.
	 */
	GeneticAlgorithm(Properties& theProperties, MTRand& random,
			CheckValidGenomeImplementer& genomeChecker);

	/**
	 * Initialize the population with random genomes. This can't be done in the constructor, since by that stage the base class's
	 * (which implement checkValidGenomeImplementer) woudn't have yet been constructed.
	 */
	void initializePopulation();
	
	/**
	 * Get the indexes of a pair of agents to run a tournament between.
	 */
	std::pair<int, int> getAgentPair();

	/**
	 * Get the indexes of a pair of agents to run a tournament between, using demes.
	 */
	std::pair<int, int> getDemeAgentPair();

	/**
	 * Run a tournament between the two specified genotypes.
	 */
	void runTournament(std::pair<int, int> agentPair);

	/**
	 * Get a genotype.
	 */
	Genotype& getGenotype(int no) {
		return myPopulation.at(no);
	}

	/**
	 * Get the number of genotypes stored.
	 */
	int getPopulationSize() {
		return myPopSize;
	}

	/** 
	 * Get the number of genes per genotype.
	 */
	int getNoGenes() {
		return myNoGenes;
	}

	/**
	 * Recalculate the mutation variance from the current best fitness value.
	 */
	void updateMutationVariance(double bestFitness);

	/**
	 * Print a matlab depiction of the population.
	 */
	std::string printPopulation(int generationNo);

	/**
	 * Adds a definition of the properties required by this class to a 
	 * Properties object.
	 */
	static void addProperties(Properties& properties);
};

#endif

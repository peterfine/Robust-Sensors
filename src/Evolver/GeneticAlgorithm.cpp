/*******************************************************************************
 * GeneticAlgorithm.cpp
 * 
 * Peter Fine - EASy MSC, Adaptive Systems Coursework
 * Created March '05
 * 
 ******************************************************************************/

using namespace std;

#include "CheckValidGenomeImplementer.h"
#include "GeneticAlgorithm.h"
#include "MersenneTwister.cpp"
#include "MatlabWriter.h"
#include "Properties.h"
#include <algorithm>
#include <math.h>
#include <sstream>

//TEMP
#include <iostream>

GeneticAlgorithm::GeneticAlgorithm(Properties& theProperties, MTRand& random, 
								   CheckValidGenomeImplementer& genomeChecker) :
	myRand(random),
	myGenomeChecker(genomeChecker),
	myPopSize(theProperties.getInt("popSize")),
	myNoGenes(theProperties.getInt("noGenes")),
	myMutationVariance(theProperties.getDouble("initialMutationVariance")),
	myRandomReplacement(theProperties.getBool("randomReplacement")),
	myUseDemes(theProperties.getBool("useDemes")),
	myDemeWidth(theProperties.getInt("demeWidth")),
	myVariableMutationVariance(theProperties.getBool("variableMutationVariance")),
	myVariableMutationVarianceMin(theProperties.getDouble("variableMutationVarianceMin")),
	myVariableMutationVarianceMax(theProperties.getDouble("variableMutationVarianceMax")),
	myVariableMutationVarianceMinFitnessThreshold(theProperties.getDouble("variableMutationVarianceMinFitnessThreshold")),
	myVariableMutationVarianceMaxFitnessThreshold(theProperties.getDouble("variableMutationVarianceMaxFitnessThreshold"))
	{ }

void GeneticAlgorithm::initializePopulation() {
	
	// Initialize the genes to random numbers between 0 and 1.
	Genotype randomGenotype(myNoGenes);

	for (int agent = 0; agent < myPopSize; agent++) {
		// (Re)fill the genotype with random genes, which conform to any required criteria.
		do {
			for (int gene = 0; gene < myNoGenes; gene++) {
				randomGenotype.setGene(gene, myRand.rand());
			}

		} while (myGenomeChecker.checkGenomeIsValid(randomGenotype) == false);

		// And put the genotype into the population array.    
		myPopulation.push_back(randomGenotype);
	}
}

pair<int, int> GeneticAlgorithm::getAgentPair() {

	if (myUseDemes) {
		return getDemeAgentPair();
	} else {
		pair<int, int> agentIndicies;
		agentIndicies.first = myRand.randInt(myPopulation.size() - 1);

		do {
			agentIndicies.second = myRand.randInt(myPopulation.size() - 1);
		} while (agentIndicies.second == agentIndicies.first);

		return agentIndicies;
	}
}

pair<int, int> GeneticAlgorithm::getDemeAgentPair() {
	pair<int, int> agentIndicies;
	agentIndicies.first = myRand.randInt(myPopulation.size() - 1);

	do {
		agentIndicies.second = (agentIndicies.first
				+ myRand.randInt(myDemeWidth) - (int)(myDemeWidth / 2.0))
				% myPopulation.size();
	} while (agentIndicies.second == agentIndicies.first);

	return agentIndicies;
}

void GeneticAlgorithm::runTournament(pair<int, int> agentPair) {

	// Check the agents aren't the same. This would cause problems when the replaced agent is set to not evaluated.
	if (agentPair.first == agentPair.second) {
		cout << "Running a tournament between an agent and itself, quitting!"
				<< endl;
		exit(1);
	}

	// Determine the winner.
	int winnerIndex;
	int loserIndex;

	if (myPopulation.at(agentPair.first).myFitness > myPopulation.at(agentPair.second).myFitness) {

		winnerIndex = agentPair.first;
		loserIndex = agentPair.second;
	} else {
		winnerIndex = agentPair.second;
		loserIndex = agentPair.first;
	}

	// Replace a member of the population with a copy of the winner.
	int replacedGenotypeIndex;
	if (myRandomReplacement) { // Select the replaced genotype randomally.	
		if (myUseDemes) {
			// Select the member to be replaced from within the current deme.
			replacedGenotypeIndex = (winnerIndex + myRand.randInt(myDemeWidth)
					- (int)(myDemeWidth / 2.0)) % myPopulation.size();
		} else {
			replacedGenotypeIndex = myRand.randInt(myPopulation.size() - 1);
		}
	} else { // Replace the losing genotype.
		replacedGenotypeIndex = loserIndex;
	}

	myPopulation.at(replacedGenotypeIndex) = myPopulation.at(winnerIndex);
	myPopulation.at(replacedGenotypeIndex).myFitness = 0.0;
	myPopulation.at(replacedGenotypeIndex).myBeenEvaluated = false;

	// Mutate that new genotype.
	mutate(myPopulation.at(replacedGenotypeIndex));
}

void GeneticAlgorithm::mutate(Genotype& genotype) {

	// Generate a dummy genotype with which to test mutations with.
	Genotype dummyGenotype = genotype;
	do { // Whilst the mutated genome are acceptable to the user...

		// Generate a dummy genotype with which to test mutations with.
		dummyGenotype = genotype;
		
		// Apply mutations to each gene in the dummy genome.
		for (int gene = 0; gene < dummyGenotype.myGenes.size(); gene++) {

			// Generate prototype mutations until one fits into [0, 1].
			double newValue = 0;
			do {
				newValue = dummyGenotype.getGene(gene) + myRand.randNorm(0.0,
						myMutationVariance);
			} while ((newValue < 0) || (newValue > 1));

			// Apply the mutation.
			dummyGenotype.setGene(gene, newValue);
		}
	} while(myGenomeChecker.checkGenomeIsValid(dummyGenotype) == false);
	// The genome is valid, so overwrite the original genotype.
	genotype = dummyGenotype;
}

void GeneticAlgorithm::updateMutationVariance(double bestFitness) {
	if (myVariableMutationVariance) {
		double proportionOfFitnessBetweenThresholds = (bestFitness
				- myVariableMutationVarianceMinFitnessThreshold)
				/ (myVariableMutationVarianceMaxFitnessThreshold
						- myVariableMutationVarianceMinFitnessThreshold);

		// Clip to [0, 1]
		if (proportionOfFitnessBetweenThresholds < 0.0)
			proportionOfFitnessBetweenThresholds = 0.0;
		if (proportionOfFitnessBetweenThresholds > 1.0)
			proportionOfFitnessBetweenThresholds = 1.0;

		// note 1 - proportionOfFitnessBetweenThresholds Is because max variance is low fitness and min is high fitness.
		myMutationVariance = (myVariableMutationVarianceMax	- myVariableMutationVarianceMin) * 
							 (1	- proportionOfFitnessBetweenThresholds)	
							 + myVariableMutationVarianceMin;
	}
}

string GeneticAlgorithm::printPopulation(int generationNo) {
	stringstream result;
	for (int i = 0; i < myPopulation.size(); i++) {
		stringstream variableName;
		variableName << "Generation" << generationNo << "_Genotype" << i + 1;
		result << MatlabWriter::parse(myPopulation.at(i).myGenes, variableName.str());
	}

	return result.str();
}

void GeneticAlgorithm::addProperties(Properties& properties) {
	// Define the properties used by the GA.
	properties.addIntItem("popSize");
	properties.addDoubleItem("initialMutationVariance");
	properties.addBoolItem("randomReplacement");
	properties.addBoolItem("useDemes");
	properties.addIntItem("demeWidth");
	properties.addBoolItem("variableMutationVariance");
	properties.addDoubleItem("variableMutationVarianceMin");
	properties.addDoubleItem("variableMutationVarianceMax");
	properties.addDoubleItem("variableMutationVarianceMinFitnessThreshold");
	properties.addDoubleItem("variableMutationVarianceMaxFitnessThreshold");
}

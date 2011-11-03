/*******************************************************************************
 * TanCTRNNetwork.h
 *
 * Peter Fine - EASy MSC, Alife Coursework
 * Created November '04
 ******************************************************************************/

#ifndef TanCTRNNetwork_h
#define TanCTRNNetwork_h

#include "NeuralNetwork.h"
#include <math.h>
#include <vector>

class Properties;
class Genotype;

/**
 * This class represents a CTRNN neural network.
 */
class TanCTRNNetwork : public NeuralNetwork {
private:

	/**
	 * The number of neurons. The first noInputs will be the input neurons,
	 * and the last noOutputs will be the outputs neurons.
	 */
	int myNoNeurons;

	/**
	 * The weight table between the different neurons.
	 */
	std::vector<double> myWeights;

	/**
	 * The neuron values.
	 */
	std::vector<double> myStates;

	/* A buffer to hold the next state of the netowkr during the update routine, so new
	 * states can be calculated whilst still maintainng the previous states for use
	 * in updating other neurons.
	 */
	std::vector<double> myNewStateBuffer;

	/**
	 * A buffer to hold the post-tanh neuron values during the update routine.
	 */
	std::vector<double> myPostTanhStateBuffer;

	/**
	 * The neuron biases.
	 */
	std::vector<double> myBiases;

	/**
	 * The neuron time constants.
	 */
	std::vector<double> myTimeConstants;

	std::vector<double> myOutputWeights;
	std::vector<double> myOutputBiases;
	
	double myTimeSinceReset;

	/**
	 *  The time step (used only to calculate the min time constant).
	 */
	double myTimeStep;

	/**
	 * Whether the network should be seeded (i.e. on a genotype's first ever construction into a network) with
	 * Centre-crossing nodes.
	 */
	bool mySeedWithCentreCrossing;

	/**
	 * Whether the network should be seeded (i.e. on a genotype's first ever construction into a network) with
	 * weights scaled by the largest eigen value.
	 */	
	bool mySeedWithScaledWeights;
	
	/**
	 * Whether the network should be constrained such that the weights are scaled by the largest eigen value.
	 */	
	bool myConstrainWithScaledWeights;
	
	/**
	 * Whether NETWORK outputs (not neuron, or this would disrupt centre crossing etc.) should be [0 1], or the default [-1 1].
	 */
	bool myScaleOutputToPositive;

	/**
	 * Whether to set the tau values to 1 or not.
	 */
	bool mySetTausToOne;
	
	bool mySetBiasesToZero;
	bool myUseOutputWeightsAndBiases;

	double myMaxWeight;
	double myInitialWeightModifier;
	bool myInitialWeightsNormal;
	
	/**
	 * The amount to add to the scaling parameter (largestEigen + this value).
	 */
	double myScaleModifier;
	
	/**
	 * Whether to use the sufficient scaling condition, or just the necessary one.
	 */
	double myUseStrictScalingCondition;
	
	/**
	 * Whether to scale the self or the non-self weights to match the strict scaling condition, per neuron.
	 */
	std::vector<bool> myStrictScalingType;
	
	/**
	 * The Sensor Weight.
	 */
	double mySensorWeight;

	/**
	 * A record of the state, which is only updated if startRecording() is called.
	 */
	std::vector<std::vector<double> > myStateRecord;

	/**
	 * A record of the input, which is only updated if startRecording() is called.
	 */
	std::vector<std::vector<double> > myInputRecord;

	/**
	 * A record of the output, which is only updated if startRecording() is called.
	 */
	std::vector<std::vector<double> > myOutputRecord;

	/**
	 * Whether to use the lookup table, or the actual tanh method for 
	 * calculating the tanh function.
	 */
	bool myUseLookup;

	/**
	 * Lookup table for the tanh function.
	 */
	std::vector<double> myTanhLookupTable;

	/**
	 * Calculate the tanh function tanh of a value.
	 * @param input The input value.
	 * @returns The result of the tanh function.
	 */
	double calcTanh(const double input);
	
	/**
	 * Scale the weights by the largest eigenvector (plus a little) to enforce an (approx. in the linear area?) null-state.
	 */
	void scaleWeights(Genotype& genotype);
	
	void writeWeightsToGenotype(Genotype& genotype);

public:

	/**
	 * Create a CTRNNNeuralNetwork with the specified properties.
	 * @param properties The properties object used to configure this network.
	 */
	TanCTRNNetwork(Properties& properties);

	/**
	 * Reconstruct the network, as specified by the provided genotype. This
	 * is specified by the NeuralNetwork base class.
	 * @param genotype The genes which specify the network.
	 */
	void reconstructNetwork(Genotype& genotype);

	/**
	 * Calculate the number of genes required to reconstruct this network.
	 */
	int getNoGenes();

	/**
	 * Update the network.
	 */
	void updateNetwork(double timeStep);

	/**
	 * Reset the neuron outputs to 0.
	 */
	void resetNetwork();

	/**
	 * Adds a definition of the properties required by this class to a 
	 * Properties object.
	 */
	static void addProperties(Properties& properties);

	/**
	 * Start recording the network's state (from NeuralNetwork).
	 */
	void resetRecords();

	/**
	 * Update the network's records.
	 */
	void updateRecords();

	/**
	 * return a string containing the neuron's state, in Matlab format (from NeuralNetwork).
	 */
	std::string getRecords();

	bool isConnected(int src, int dest) {
		return false;
	}
	
	/**
	 * Check to see if the genome applies to any stability constraints which may or may not be applicable.
	 */
	bool checkGenomeIsValid(Genotype& aGenotype);

};

#endif // TanCTRNNetwork_h

/*******************************************************************************
 * TanCTRNNetwork.cpp
 * 
 * Peter Fine - EASy MSC, Alife Coursework
 * Created November '04
 ******************************************************************************/

using namespace std;

#include "TanCTRNNetwork.h"
#include "MatlabWriter.h"
#include "Properties.h"
#include "Genotype.h"
#include "tnt126/tnt.h"
#include "jama125/jama_eig.h"

// TEMP
#include <iostream>
#include <sstream>

TanCTRNNetwork::TanCTRNNetwork(Properties& properties) :
			NeuralNetwork(properties.getInt("noInputs"), properties.getInt("noOutputs")),
			myNoNeurons(myNoInputs + properties.getInt("noInterneurons") + myNoOutputs),
			myNewStateBuffer(myNoNeurons),
			myStates(myNoNeurons),
			myPostTanhStateBuffer(myNoNeurons),
			myWeights(myNoNeurons * myNoNeurons),
			myBiases(myNoNeurons),
			myTimeConstants(myNoNeurons),
			myOutputWeights(myNoOutputs),
			myOutputBiases(myNoOutputs),
			myTimeStep(properties.getDouble("timestep")),
			mySeedWithCentreCrossing(properties.getBool("seedWithCentreCrossing")),
			myConstrainToStable(properties.getBool("constrainToStable")),
			myUseLookup(properties.getBool("useLookup")),
			myScaleOutputToPositive(properties.getBool("scaleOutputToPositive")),
			mySetTausToOne(properties.getBool("setTanhTausToOne")),
			mySetBiasesToZero(properties.getBool("setBiasesToZero")),
			myUseOutputWeightsAndBiases(properties.getBool("useOutputWeightsAndBiases")),
			myMaxWeight(properties.getDouble("maxWeight")),
			myInitialWeightModifier(properties.getDouble("initialWeightModifier")),
			myNetworkIsFeedforward(properties.getBool("networkIsFeedforward")),
			myFFLayer1Size(properties.getInt("ffLayer1Size")),
			myFFLayer2Size(properties.getInt("ffLayer2Size")),
			myLinearTransfer(properties.getBool("linearTransfer"))
			{
	
	// Initialize the tanh lookup table.
	for (double val = -10; val <= 10; val+= 0.002) {
		myTanhLookupTable.push_back(tanh(val));
	}
	
	// Check feedforward structure is correct.
	if(myNetworkIsFeedforward && (myNoInputs + myFFLayer1Size + myFFLayer2Size + myNoOutputs != myNoNeurons)) {
		cout << "Feedforward configured wrongly" << endl;
		exit(1);
	}
}

double TanCTRNNetwork::calcTanh(const double input) {

	if (myUseLookup) {
		/* ROUNDED */
		int index = (int)round((input / 0.002) + 5000.0);
		if (index < 0) {
			return -1.0;
		} else if (index > 9999) {
			return 1.0;
		} else {
			return myTanhLookupTable[index];
		}
	} else {
		/* EXACT */
		return tanh(input);
	}
}

void TanCTRNNetwork::reconstructNetwork(Genotype& genotype) {

	//  Initialise the parameters according to the genotype.
	int gene = 0;

	/* Reconstruct the weights between the neurons.
	 * Note these GENE NUMBERS are expected to be at the begining of the genome by 
	 * writeWeightsToGenome.
	 */
	for (int dest = 0; dest < myNoNeurons; dest++) {
		for (int source = 0; source < myNoNeurons; source++) {
			// Note inputs now have incoming weights. Is this good?
			myWeights[dest * myNoNeurons + source] = genotype.myGenes.at(gene) * myMaxWeight * 2 - myMaxWeight;
			gene++;
		}
	}

	// If requested, on first construction remap the inital weights to an alternative
	// bounds.
	if((genotype.myHasEverBeenConstructed == false) &&
	   (abs(myInitialWeightModifier - myMaxWeight) > 0.000001)) {
		
		for (int dest = 0; dest < myNoNeurons; dest++) {
			for (int source = 0; source < myNoNeurons; source++) {
				myWeights[dest * myNoNeurons + source] = myWeights[dest * myNoNeurons + source]
				                                         * (myInitialWeightModifier / myMaxWeight);
			}
		}
		// And write these weights back into the genenome.
		writeWeightsToGenotype(genotype);
	}
	
	// If requested, impose a feedforward structure.
	if(myNetworkIsFeedforward) {
		for (int dest = 0; dest < myNoNeurons; dest++) {
			for (int source = 0; source < myNoNeurons; source++) {
				int destLayer, sourceLayer;
				if(dest < myNoInputs) { destLayer = 0; }
				else if(dest < myNoInputs + myFFLayer1Size) { destLayer = 1; }
				else if(dest < myNoInputs + myFFLayer1Size + myFFLayer2Size) { destLayer = 2; }
				else { destLayer = 3; }
				if(source < myNoInputs) { sourceLayer = 0; }
				else if(source < myNoInputs + myFFLayer1Size) { sourceLayer = 1; }
				else if(source < myNoInputs + myFFLayer1Size + myFFLayer2Size) { sourceLayer = 2; }
				else { sourceLayer = 3; }
				
				if(sourceLayer != (destLayer - 1)) {
					myWeights[dest * myNoNeurons + source] = 0.0;
				}
			}
		}
	}
	
	// Sensor weights.
	mySensorWeight = genotype.myGenes.at(gene) * myMaxWeight * 2 - myMaxWeight;
	gene++;

	// Output weights and biases (if required).
	if(myUseOutputWeightsAndBiases) {
		for(int o = 0; o < myNoOutputs; o++) {
			myOutputWeights[o] = genotype.myGenes.at(gene) * myMaxWeight * 2 - myMaxWeight;
			gene++;
			myOutputBiases[o] = genotype.myGenes.at(gene) * myMaxWeight * 2 - myMaxWeight;
			gene++;
		}
	}

	/* If the population should be seeded with centre-crossing networks, and the network has never been constructed
	 * before, ALTER THE GENES so the bias genes are centre crossing.
	 */
	for (int i = 0; i < myNoNeurons; i++) {
		if (mySeedWithCentreCrossing && genotype.myHasEverBeenConstructed == false) {
			genotype.setGene(gene, 0.5); // XXX IS THIS RIGHT FOR CENTRE CROSSING IN TANH???
		}
		
		myBiases[i] = genotype.myGenes.at(gene) * myMaxWeight * 2 - myMaxWeight;
		gene++;

		// Set time constants, scaled exponentially to [exp(0) exp(5)]
		myTimeConstants[i] = exp(genotype.myGenes.at(gene) * 5);
		gene++;
		
		// If requested, actually set the time constants to 1.
		if(mySetTausToOne) { myTimeConstants[i] = 1.0; }
		
		// If requested, actually set the biases to 0.
		if(mySetBiasesToZero) { myBiases[i] = 0.0; }
	}
	
	// Record in the genotype that it has been reconstructed.
	genotype.myHasEverBeenConstructed = true;
}

bool TanCTRNNetwork::checkGenomeIsValid(Genotype& aGenotype) {
	
	if(myConstrainToStable == false) {
		// Always valid, since we're either not constraining the network.
		return true;
	}
	
	/* Create a copy of the genome, which can be constructed without having to worry
	 * about whether it changes the genome at all.
	 */
	Genotype dummyGenotype = aGenotype;
	reconstructNetwork(dummyGenotype);
	
	// Check whether the weight matrix is stable. First, calculate the full jacobian...
	// Put the weights in the format required by the JAMA eigen decomposition class.
	TNT::Array2D<double> jacobianTNT(myNoNeurons, myNoNeurons);
	for (int source = 0; source < myNoNeurons; source++) {
		for (int dest = 0; dest < myNoNeurons; dest++) {
			double identityModifier = 0.0;
			if(dest == source) { identityModifier = 1.0; }
			
				// XXX RIGHT WAY ROUND ??? Don't think it matters!
				jacobianTNT[source][dest] = (myWeights[dest * myNoNeurons + source] - identityModifier) / 
											myTimeConstants[dest];	
		}
	}
	
	// Get the largest real positive eigenvalues.
	JAMA::Eigenvalue<double> eigen(jacobianTNT);
	TNT::Array1D<double> eigenValues;
	eigen.getRealEigenvalues(eigenValues);
	
	double largestEigen = eigenValues[0]; 
	for(int i = 1; i < myNoNeurons; i++) { // Note starts at 1 due to largestEigen's initialization above.
		if(eigenValues[i] > largestEigen) {
			largestEigen = eigenValues[i];
		}
	}

	// Return true (i.e. the genome is valid) if the largest eigen value is less than 0.
	if(largestEigen < 0.0) {
		return true;
	}
	else {
		return false;
	}
}

int TanCTRNNetwork::getNoGenes() {
	
	int noOutputGenes = 0;
	if(myUseOutputWeightsAndBiases) { noOutputGenes += 2 * myNoOutputs;	}
	
	return myNoNeurons * myNoNeurons // Weights.
			+ myNoNeurons // Biases.
			+ myNoNeurons // Time Constants
			+ noOutputGenes // If requested.
			+ 1; // Sensor Weight.
}

void TanCTRNNetwork::resetNetwork() {

	// Reset all inputs to 0.
	for (int i = 0; i < myNoInputs; i++) {
		myInputs[i] = 0.0;
	}

	// Reset all states to 0.
	for (int i = 0; i < myNoNeurons; i++) {
		myStates[i] = 0.0;
	}

	// Reset all outputs to 0.
	for (int i = 0; i < myNoOutputs; i++) {
		myOutputs[i] = 0.0;
	}

	myTimeSinceReset = 0;
}

// UPDATE -----------------------------------------------------------------------------------------------------
void TanCTRNNetwork::updateNetwork(double timeStep) {
	myTimeSinceReset += timeStep;

	// If Feedforward, use a timestep of 1. BE CAREFUL WITH OTHER USES OF TS!
	if(myNetworkIsFeedforward) { timeStep = 1.0; }
	
	/* First record each neuron's post-tanh value, for efficiency.
	 */
	for (int i = 0; i < myNoNeurons; i++) {
		if(myLinearTransfer) {
			myPostTanhStateBuffer[i] = myStates[i] + myBiases[i];
		}
		else {
			myPostTanhStateBuffer[i] = calcTanh(myStates[i] + myBiases[i]);
		}
	}

	for (int dest = 0; dest < myNoNeurons; dest++) {
		// Calculate the inputs.
		double input = 0.0;
		for (int src = 0; src < myNoNeurons; src++) {
			input += myWeights[dest * myNoNeurons + src] * myPostTanhStateBuffer[src];
		}

		// Apply external inputs.
		if (dest < myNoInputs) {
			// Apply inputs.
			input += mySensorWeight * myInputs.at(dest);
		}

		// Update the neuron's state.
		myNewStateBuffer[dest] = myStates[dest] + (timeStep
				/ myTimeConstants[dest]) * (-myStates[dest] + input);
	}

	// Update the neuron output records.
	myStates = myNewStateBuffer;

	// Record the current neuron outputs.
	for (int o = 0; o < myNoOutputs; o++) {
		if(myUseOutputWeightsAndBiases) {
			//XXXSORT THIS OUT!
			myOutputs[o] = calcTanh(myOutputWeights.at(o) * myStates.at(myNoNeurons - myNoOutputs + o) // SHOULD BE PLUS OUTPUT NEURON BIAS TOO perhaps? Chris thinks not maybe.
									+ myOutputBiases.at(o));
		}
			else {
		myOutputs[o] = calcTanh(myStates.at(myNoNeurons - myNoOutputs + o)
				+ myBiases.at(myNoNeurons - myNoOutputs + o));
			}
		
		if(myScaleOutputToPositive) {
			myOutputs[o] = (myOutputs[o] + 1.0) * 0.5;
		}
	}
}

void TanCTRNNetwork::writeWeightsToGenotype(Genotype& genotype) {
	// Write these weights back into the genotype.
	int geneNo = 0;
	for (int dest = 0; dest < myNoNeurons; dest++) {
		for (int source = 0; source < myNoNeurons; source++) {
			// Note inputs now have incoming weights. Is this good?
			genotype.myGenes.at(geneNo) = (myWeights[dest * myNoNeurons + source] + myMaxWeight) / (2*myMaxWeight);
			if((genotype.myGenes.at(geneNo) < 0) || (genotype.myGenes.at(geneNo) > 1)) {
				// Gene is out of bounds, quit!
				cout << "Gene is out of bounds on writing, quitting!" << endl;
				exit(1);
			}
			geneNo++;
		}
	}
}

void TanCTRNNetwork::resetRecords() {

	// Clear the state containers.
	myStateRecord.clear();
	myInputRecord.clear();
	myOutputRecord.clear();
}

void TanCTRNNetwork::updateRecords() {
	myStateRecord.push_back(myStates);
	myInputRecord.push_back(myInputs);
	myOutputRecord.push_back(myOutputs);
}

string TanCTRNNetwork::getRecords(bool includeState) {
	// Output the network configuration and the state since recording started.
	stringstream record;

	record << MatlabWriter::parse(myBiases, "bias");
	record << MatlabWriter::parse(myTimeConstants, "tau");
	
	record << MatlabWriter::parse(myUseOutputWeightsAndBiases, "useOutputWeightsAndBiases");
	if(myUseOutputWeightsAndBiases) {
		record << MatlabWriter::parse(myOutputWeights, "outputWeights");
		record << MatlabWriter::parse(myOutputBiases, "outputBiases");
	}
	
	// Read the weights into a 2D matrix for recording.
	vector<vector<double> > weights2D(myNoNeurons, vector<double>(myNoNeurons));
	for(int src = 0; src < myNoNeurons; src++) {
		for(int dest = 0; dest < myNoNeurons; dest++) {
			weights2D.at(src).at(dest) = myWeights.at(dest * myNoNeurons + src);
		}	
	}
	record << MatlabWriter::parse(weights2D, "weights");
	record << MatlabWriter::parse(mySensorWeight, "sensorWeight");
	record << MatlabWriter::parse(myUseLookup, "useLookup");
	
	record << MatlabWriter::parse(myMaxWeight, "maxWeight");
	record << MatlabWriter::parse(myInitialWeightModifier, "initialWeightModifier");
	
	record << MatlabWriter::parse(myNetworkIsFeedforward, "networkIsFeedforward");
	if(myNetworkIsFeedforward) {
		record << MatlabWriter::parse(myFFLayer1Size, "ffLayer1Size");
		record << MatlabWriter::parse(myFFLayer2Size, "ffLayer2Size");
	}

	if(includeState) {
		record << MatlabWriter::parse(myStateRecord, "neuronStates");
		record << MatlabWriter::parse(myInputRecord, "neuronInputs");
		record << MatlabWriter::parse(myOutputRecord, "neuronOutputs");
	}
	
	return record.str();
}

void TanCTRNNetwork::addProperties(Properties& properties) {
	properties.addIntItem("noInterneurons");
	properties.addBoolItem("seedWithCentreCrossing");
	properties.addBoolItem("constrainToStable");
	properties.addBoolItem("useLookup");
	properties.addBoolItem("scaleOutputToPositive");
	properties.addBoolItem("setTanhTausToOne");
	properties.addBoolItem("setBiasesToZero");
	properties.addBoolItem("useOutputWeightsAndBiases");
	properties.addDoubleItem("maxWeight");
	properties.addDoubleItem("initialWeightModifier");
	properties.addBoolItem("networkIsFeedforward");
	properties.addIntItem("ffLayer1Size");
	properties.addIntItem("ffLayer2Size");
	properties.addBoolItem("linearTransfer");
}

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
			mySeedWithScaledWeights(properties.getBool("seedWithScaledWeights")),
			myConstrainWithScaledWeights(properties.getBool("constrainWithScaledWeights")),
			myUseLookup(properties.getBool("useLookup")),
			myScaleOutputToPositive(properties.getBool("scaleOutputToPositive")),
			mySetTausToOne(properties.getBool("setTanhTausToOne")),
			mySetBiasesToZero(properties.getBool("setBiasesToZero")),
			myUseOutputWeightsAndBiases(properties.getBool("useOutputWeightsAndBiases")),
			myScaleModifier(properties.getDouble("scaleModifier")),
			myUseStrictScalingCondition(properties.getBool("strictScalingCondition")),
			myStrictScalingType(myNoNeurons),
			myMaxWeight(properties.getDouble("maxWeight")),
			myInitialWeightModifier(properties.getDouble("initalWeightModifier")),
			myInitialWeightsNormal(properties.getBool("initialWeightsNormal"))
			{
	
	// Check that we're not both seeding and constraining the scaled weights.
	if(mySeedWithScaledWeights && myConstrainWithScaledWeights) {
		cout << "Both mySeedWithScaledWeights and myConstrainWithScaledWeights were true, quitting!" << endl;
		exit(1);
	}
	
	// Initialize the tanh lookup table.
	for (double val = -10; val <= 10; val+= 0.002) {
		myTanhLookupTable.push_back(tanh(val));
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

	/* INTERPOLATED - NOT RECODED FOR TANH
	 double exactIndex = (input / 0.002) + 5000;
	 int index = (int)floor(exactIndex);	
	 if(index < 0.0) { return 0.0; }
	 else if (index > 9998) { return 1.0; }
	 else {
	 double resLower = mySigmoidLookupTable.at(index);
	 double resUpper = mySigmoidLookupTable.at(index+1);
	 return resLower + (exactIndex - index) * (resUpper - resLower);
	 }
	 */
}

void TanCTRNNetwork::reconstructNetwork(Genotype& genotype) {

	//  Initialise the parameters according to the genotype.
	int gene = 0;

	/* Reconstruct the weights between the neurons (ommiting inputs to the sensory
	 * neurons). Note these GENE NUMBERS and BOUNDS are expected to be at the begining of the genome by the
	 * weight scaling algorithm below.
	 */
	for (int dest = 0; dest < myNoNeurons; dest++) {
		for (int source = 0; source < myNoNeurons; source++) {
			// Note inputs now have incoming weights. Is this good?
			if(myConstrainWithScaledWeights && (dest == source)) {
				if(genotype.myHasEverBeenConstructed == false) {
					genotype.myGenes.at(gene) = 0.0;
				} //XXX NOTE SELF WEIGHTS NEGATIVE, if changed, change saving of genes after scaling as well!
				myWeights[dest * myNoNeurons + source] = -1 * genotype.myGenes.at(gene) * myMaxWeight;
			}
				else {
					myWeights[dest * myNoNeurons + source] = genotype.myGenes.at(gene) * myMaxWeight * 2 - myMaxWeight;
				}
			gene++;
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
		
		// If the weights are to be scaled, and the strict condition is being used, then an array of the scaling type for each neuron is evolved.
		if(myConstrainWithScaledWeights && myUseStrictScalingCondition) {
			if(genotype.myGenes.at(gene) < 0.5) { 
				myStrictScalingType[i] = true; 
			}
				else { 
					myStrictScalingType[i] = false; 
				}
			gene++;
		}
	}

	// Scale the weights if required. ---------------------------------------------------------------------------------------
	// First, determine whether the weights should be scaled or not.
	
	// Scale the weights once, an write them back into the genome.
	if (mySeedWithScaledWeights && (genotype.myHasEverBeenConstructed == false)) {		// Note must have initialized time constants before running this!
		scaleWeights(genotype);
		
		// Write the weights back into the genome.
		int weightGeneNo = 0;
		for (int dest = 0; dest < myNoNeurons; dest++) {
			for (int source = 0; source < myNoNeurons; source++) {
				// Note inputs now have incoming weights. Is this good?
				genotype.myGenes.at(weightGeneNo) = (myWeights[dest * myNoNeurons + source] + myMaxWeight) / (2 * myMaxWeight);
				// and clip. // XXX DOES THIS OFTEN HAPPEN???
				if(genotype.myGenes.at(weightGeneNo) > 1.0) genotype.myGenes.at(weightGeneNo) = 1.0;
				if(genotype.myGenes.at(weightGeneNo) < 0.0) genotype.myGenes.at(weightGeneNo) = 0.0;

				weightGeneNo++;
			}
		}
	}
	
	// Scale the weights every time, but do not write them back into the genome.
	if(myConstrainWithScaledWeights) scaleWeights(genotype); // XXX Does the centrecrossing-ness need to be reaplied for this to work?
	
	// Record in the genotype that it has been reconstructed.
	genotype.myHasEverBeenConstructed = true;
}

// Only used by the necessary condition, not the sufficient one.
bool TanCTRNNetwork::checkGenomeIsValid(Genotype& aGenotype) {
	
	if((myConstrainWithScaledWeights == false) || myUseStrictScalingCondition) {
		// Always valid, since we're either not scaling weights, or using a different, stricter condition.
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
	int genesForStrictScalingType = 0;
	if(myUseStrictScalingCondition) { genesForStrictScalingType += myNoNeurons; }
	
	int noOutputGenes = 0;
	if(myUseOutputWeightsAndBiases) { noOutputGenes += 2 * myNoOutputs;	}
	
	return myNoNeurons * myNoNeurons // Weights.
			+ myNoNeurons // Biases.
			+ myNoNeurons // Time Constants.
			+ genesForStrictScalingType // strictScalingType (not always included).
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

	/* First record each neuron's post-tanh value, for efficiency.
	 */
	for (int i = 0; i < myNoNeurons; i++) {
		myPostTanhStateBuffer[i] = calcTanh(myStates[i] + myBiases[i]);
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
			myOutputs[o] = calcTanh(myOutputWeights.at(o) * myStates.at(myNoNeurons - myNoOutputs + o)
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

// Scale weight by the largest eigenvalue. Must have initialized time constants too!
void TanCTRNNetwork::scaleWeights(Genotype& genotype) {
	return;
	// Use the strict (i.e. sufficient) scaling type --------------------------------------------------------------
	if(myUseStrictScalingCondition) {
		// Scale each row (i.e. each set of outgoing weights) such that Wii + 0.5 * sumJ(j != i [ |Wij| + |Wji|] ) < -1   for all i.
		for (int src = 0; src < myNoNeurons; src++) {
			
			double selfWeight = myWeights[src * myNoNeurons + src];
			double sum = 0.0;
			for (int dest = 0; dest < myNoNeurons; dest++) {
				if(src != dest) {
					sum += abs(myWeights[dest * myNoNeurons + src]) + abs(myWeights[src * myNoNeurons + dest]);
				}
			}
			
			if((selfWeight + 0.5*sum) < -1) {
				// Scale the appropriate side of the equation.
				if(myStrictScalingType[src]) {
					// Scale the self weight.
					exit(1);
				}
				else {
					// Scale the other weights.
					exit(1);
				}
			}
			// WRITE BACK INTO GENOME HERE
			//myWeights[dest * myNoNeurons + src]
		}
	}
	
	// Use the necessary (i.e. not necessarily sufficient) scaling type -----------------------------------------
	else {
			
		// Put the weights in the format required by the JAMA eigen decomposition class.
		TNT::Array2D<double> weightMatrixTNT(myNoNeurons, myNoNeurons);
		for (int dest = 0; dest < myNoNeurons; dest++) {
			for (int source = 0; source < myNoNeurons; source++) {
				// XXX RIGHT WAY ROUND ??? Don't think it matters!
				weightMatrixTNT[source][dest] = myWeights[dest * myNoNeurons + source];
			}	
		}
	
		// Get the largest eigenvalues.
		JAMA::Eigenvalue<double> eigen(weightMatrixTNT);
		TNT::Array1D<double> eigenValues;
		eigen.getRealEigenvalues(eigenValues);
		double largestEigen = -99999999;
		for(int i = 0; i < myNoNeurons; i++) {
			if(eigenValues[i] > largestEigen) {
				largestEigen = eigenValues[i];
			}
		}
		
		// Scale the weights by the largest eigenvector (plus a little), only if that eigenvalue was greater than 1.
		if(largestEigen >= 1.0) {
			for (int dest = 0; dest < myNoNeurons; dest++) {
				for (int source = 0; source < myNoNeurons; source++) {
					myWeights[dest * myNoNeurons + source] = myWeights[dest * myNoNeurons + source] / (largestEigen + myScaleModifier); // XXX RIGHT WAY ROUND ???
				}
			}
		}
		
		// Write these weights back into the genotype.
		writeWeightsToGenotype(genotype);
	}
}

void TanCTRNNetwork::writeWeightsToGenotype(Genotype& genotype) {
	// Write these weights back into the genotype.
	int geneNo = 0;
	for (int dest = 0; dest < myNoNeurons; dest++) {
		for (int source = 0; source < myNoNeurons; source++) {
			// Note inputs now have incoming weights. Is this good?
			if(myConstrainWithScaledWeights && (dest == source)) {
				// Self weight is negative.
				genotype.myGenes.at(geneNo) = myWeights[dest * myNoNeurons + source] / -myMaxWeight;
			}
				else {
					genotype.myGenes.at(geneNo) = (myWeights[dest * myNoNeurons + source] + myMaxWeight) / (2*myMaxWeight);
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

string TanCTRNNetwork::getRecords() {
	// Output the network configuration and the state since recording started.
	stringstream record;

	record << MatlabWriter::parse(myBiases, "bias");
	record << MatlabWriter::parse(myTimeConstants, "tau");
	record << MatlabWriter::parse(myScaleModifier, "scaleModifier");
	record << MatlabWriter::parse(myUseStrictScalingCondition, "strictScalingCondition");
	
	if(myUseStrictScalingCondition) {
		record << MatlabWriter::parse(myStrictScalingType, "strictScalingType");
	}
	
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
	record << MatlabWriter::parse(myStateRecord, "neuronStates");
	record << MatlabWriter::parse(myInputRecord, "neuronInputs");
	record << MatlabWriter::parse(myOutputRecord, "neuronOutputs");
	
	record << MatlabWriter::parse(myMaxWeight, "maxWeight");
	record << MatlabWriter::parse(myInitialWeightModifier, "initialWeightModifier");
	record << MatlabWriter::parse(myInitialWeightsNormal, "initalWeightsNormal");

	return record.str();
}

void TanCTRNNetwork::addProperties(Properties& properties) {
	properties.addIntItem("noInterneurons");
	properties.addBoolItem("seedWithCentreCrossing");
	properties.addBoolItem("seedWithScaledWeights");
	properties.addBoolItem("constrainWithScaledWeights");
	properties.addBoolItem("useLookup");
	properties.addBoolItem("scaleOutputToPositive");
	properties.addBoolItem("setTanhTausToOne");
	properties.addDoubleItem("scaleModifier");
	properties.addBoolItem("strictScalingCondition");
	properties.addBoolItem("setBiasesToZero");
	properties.addBoolItem("useOutputWeightsAndBiases");
	properties.addDoubleItem("maxWeight");
	properties.addDoubleItem("initialWeightModifier");
	properties.addDoubleItem("initalWeightsNormal");
}

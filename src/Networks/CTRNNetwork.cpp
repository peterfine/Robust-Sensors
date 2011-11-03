/*******************************************************************************
 * CTRNNetwork.cpp
 * 
 * Peter Fine - EASy MSC, Alife Coursework
 * Created November '04
 ******************************************************************************/

using namespace std;

#include "CTRNNetwork.h"
#include "MatlabWriter.h"
#include "Properties.h"
#include "Genotype.h"

// TEMP
#include <iostream>
#include <sstream>

CTRNNetwork::CTRNNetwork(Properties& properties) :
			NeuralNetwork(properties.getInt("noInputs"), properties.getInt("noOutputs")),
			myNoNeurons(myNoInputs + properties.getInt("noInterneurons") + myNoOutputs),
			myNewStateBuffer(myNoNeurons),
			myStates(myNoNeurons),
			myPostSigmoidStateBuffer(myNoNeurons),
			myWeights(myNoNeurons * myNoNeurons),
			myBiases(myNoNeurons),
			myGains(myNoNeurons),
			myTimeConstants(myNoNeurons),
			myTimeStep(properties.getDouble("timestep")),
			mySeedWithCentreCrossing(properties.getBool("seedWithCentreCrossing")),
			myUseLookup(properties.getBool("useLookup")) {
	
	// Initialize the sigmoid lookup table.
	for (double val = -10; val <= 10; val+= 0.002) {
		mySigmoidLookupTable.push_back(1 / (1 + exp(-val)));
	}
}

double CTRNNetwork::sigmoid(const double input) {

	if (myUseLookup) {
		/* ROUNDED */
		int index = (int)round((input / 0.002) + 5000.0);
		if (index < 0) {
			return 0.0;
		} else if (index > 9999) {
			return 1.0;
		} else {
			return mySigmoidLookupTable[index];
		}
	} else {
		/* EXACT */
		return 1 / (1 + exp(-input));
	}

	/* INTERPOLATED 
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

void CTRNNetwork::reconstructNetwork(Genotype& genotype) {

	//  Initialise the parameters according to the genotype.
	int gene = 0;

	/* Reconstruct the weights between the neurons (ommiting inputs to the sensory
	 * neurons).
	 */
	for (int dest = 0; dest < myNoNeurons; dest++) {
		for (int source = 0; source < myNoNeurons; source++) {
			//      if(dest < myNoInputs) {  // Input neurons now have incoming weights.
			// Input neurons have no neuronal inputs.
			//        myWeights[dest * myNoNeurons + source] = 0.0;
			//     }
			//       else {   	
			myWeights[dest * myNoNeurons + source] = genotype.myGenes.at(gene)
					* 20.0 - 10.0;
			gene++;
			//      }
		}
	}

	// Sensor weights.
	mySensorWeight = genotype.myGenes.at(gene) * 20.0 - 10.0;
	gene++;

	/* If the population should be seeded with centre-crossing networks, and the network has never been constructed
	 * before, ALTER THE GENES so the bias genes are centre crossing.
	 */
	bool createCentreCrossingBiases = false;
	if (genotype.myHasEverBeenConstructed == false) {
		// This genotype has never been constructed before.
		if (mySeedWithCentreCrossing) {
			createCentreCrossingBiases = true;
		}
	}

	for (int i = 0; i < myNoNeurons; i++) {
		if (createCentreCrossingBiases) {
			// Determine the centre-crossing bias value.
			double totalIncomingWeights = 0;
			for (int src = 0; src < myNoNeurons; src++) {
				totalIncomingWeights += myWeights[i * myNoNeurons + src];
			}
			double newBias = (-totalIncomingWeights) / 2.0;

			// Write the centre-crossing bias to the genotype, mapping it back to [0 1].
			double newBiasGene = (newBias + 10.0) / 20.0;
			// Crop this gene to [0 1], making it not strictly 100% centre crossing.
			if (newBiasGene < 0.0) newBiasGene = 0.0;
			if (newBiasGene > 1.0) newBiasGene = 1.0;

			genotype.setGene(gene, newBiasGene);
		}
		myBiases[i] = genotype.myGenes.at(gene) * 20.0 - 10.0;
		gene++;

		// Set time constants.
		// Scale exponentially to [exp(0) exp(5)]
		//		if(i >= myNoInputs) {	
		myTimeConstants[i] = exp(genotype.myGenes.at(gene) * 5);
		gene++;
		//		}

		// Gains default to 1.
		myGains[i] = 1.0;
	}

	// Use a single time constant and gain for all input neurons.
	//	double sensorTimeConstant = exp(genotype.myGenes.at(gene) * 5);
	//gene++;

	//	double sensorGain = 1; // Gains are disabled by this. was = genotype.myGenes.at(gene) * 5.0; gene ++;
	for (int i = 0; i < myNoInputs; i++) {
		//		myTimeConstants[i] = sensorTimeConstant;
		//		myGains[i] = sensorGain;
	}

	// Record in the genotype that it has been reconstructed.
	genotype.myHasEverBeenConstructed = true;
}

int CTRNNetwork::getNoGenes() {
	return myNoNeurons * myNoNeurons // Weights.
			+ myNoNeurons // Biases.
			+ myNoNeurons // Time Constants. 
			+ 1; // Sensor Weight.
}

void CTRNNetwork::resetNetwork() {

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
void CTRNNetwork::updateNetwork(double timeStep) {

	myTimeSinceReset += timeStep;

	/* First record each neuron's post-sigmoid value, for efficiency.
	 */
	for (int i = 0; i < myNoNeurons; i++) {
		myPostSigmoidStateBuffer[i] = sigmoid(myStates[i] + myBiases[i]);
	}

	for (int dest = 0; dest < myNoNeurons; dest++) {
		// Calculate the inputs.
		double input = 0.0;
		for (int src = 0; src < myNoNeurons; src++) {
			input += myWeights[dest * myNoNeurons + src]
					* myPostSigmoidStateBuffer[src];
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
		myOutputs[o] = sigmoid(myStates.at(myNoNeurons - myNoOutputs + o)
				+ myBiases.at(myNoNeurons - myNoOutputs + o));
	}
}

void CTRNNetwork::resetRecords() {

	// Clear the state containers.
	myStateRecord.clear();
	myInputRecord.clear();
	myOutputRecord.clear();
}

void CTRNNetwork::updateRecords() {
	myStateRecord.push_back(myStates);
	myInputRecord.push_back(myInputs);
	myOutputRecord.push_back(myOutputs);
}

string CTRNNetwork::getRecords(bool includeState) {
	// Output the network configuration and the state since recording started.
	stringstream record;

	if(includeState) {
		record << MatlabWriter::parse(myStateRecord, "neuronStates");
		record << MatlabWriter::parse(myInputRecord, "neuronInputs");
		record << MatlabWriter::parse(myOutputRecord, "neuronOutputs");
	}
	record << MatlabWriter::parse(myBiases, "bias");
	record << MatlabWriter::parse(myTimeConstants, "tau");
	record << MatlabWriter::parse(myGains, "gain");
	record << MatlabWriter::parse(myWeights, "weights");
	record << MatlabWriter::parse(mySensorWeight, "sensorWeight");

	return record.str();
}

void CTRNNetwork::addProperties(Properties& properties) {
	properties.addIntItem("noInterneurons");
	properties.addBoolItem("seedWithCentreCrossing");
	properties.addBoolItem("useLookup");
}

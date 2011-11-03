
using namespace std;

#include "NetworkFactory.h"
#include "NeuralNetwork.h"
#include "CTRNNetwork.h"
#include "TanCTRNNetwork.h"
#include "HomeostaticBraitenburgNetwork.h"
#include "Properties.h"
#include "MersenneTwister.cpp"

#include <iostream>

NeuralNetwork* NetworkFactory::generateNetwork(Properties& theProperties, MTRand& theRand) {
	NeuralNetwork* theNetwork;
	
	// Set the number of inputs and outputs.
	theProperties.addIntItem("noInputs", 1);

  theProperties.addIntItem("noOutputs", 2);
	// Determine which network was specified, and construct it.
	if(theProperties.getString("networkType") == "CTRNN") {
		theNetwork = new CTRNNetwork(theProperties);
	}
	else if(theProperties.getString("networkType") == "HomeostaticBraitenburg") {
		theNetwork = new HomeostaticBraitenburgNetwork(theProperties, theRand);
	}
	else if(theProperties.getString("networkType") == "TanCTRNN") {
		theNetwork = new TanCTRNNetwork(theProperties);
	}	
		else {
			cout << theProperties.getString("networkType")
					 << " not a recognized type."
					 << endl;
			exit(1);
		}

	return theNetwork;										 	
}
																
void NetworkFactory::addProperties(Properties& theProperties) {
			
	theProperties.addStringItem("networkType");
	
	// Add all the properties which may be required to the database.
	CTRNNetwork::addProperties(theProperties);
	TanCTRNNetwork::addProperties(theProperties);
	HomeostaticBraitenburgNetwork::addProperties(theProperties);
}

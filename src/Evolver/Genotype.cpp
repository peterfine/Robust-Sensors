/*******************************************************************************
 * Genotype.cpp
 * 
 * Peter Fine - EASy MSC, Adaptive Systems Coursework
 * Created March '05
 * 
 ******************************************************************************/

#include "Genotype.h"
#include "Properties.h"
#include <iostream>
#include <sstream>

Genotype::Genotype(int noGenes) :
	myGenes(noGenes),
  myBeenEvaluated(false),
  myHasEverBeenConstructed(false)
  { }
  
Genotype::Genotype(Properties& theProperties) {
	
	// Reconstruct the geneotype from the properties object.
	int noGenes = theProperties.getInt("noGenes");
	vector<double> genesVec;
	for(int i = 0; i < noGenes; i++) {
		// Create a string matching this gene's label.
		std::stringstream label;
    label << "Gene" << i;
    // Lookup this label in the properties object to find the gene.
    myGenes.push_back(theProperties.getDouble(label.str()));
	}
	
	myFitness = theProperties.getDouble("Genotype_Fitness");
	
	/* Set myBeenEvaluated settings as false, since the agent hasn't been evaluated since reconstruction.
	 */
	myBeenEvaluated = false;

	// Determine from the properties object whether the genotype has already been reconstructed or not.
	myHasEverBeenConstructed = theProperties.getBool("Genotype_HasEverBeenConstructed");
	
	// If it has never been constructed, it is strange that was ever saved to a file. Warn about this and quit!
	if(myHasEverBeenConstructed == false) {
		cout << "RECONSTRUCTED GENOTYPE HAS NEVER BEEN CONSTRUCTED. THIS IS STRANGE, QUITTING..." << endl;
		exit(1);
	}
}

void Genotype::write(Properties& theProperties) {

  for(int i = 0; i < myGenes.size(); i++) {
  	stringstream label;
  	label << "Gene" << i;
   	theProperties.addDoubleItem(label.str(), myGenes.at(i));
  }

	theProperties.addDoubleItem("Genotype_Fitness", myFitness);
	theProperties.addBoolItem("Genotype_HasEverBeenConstructed", myHasEverBeenConstructed);
}

void Genotype::setGene(int geneNo, double newValue) { 
	if((newValue < 0) || (newValue > 1)) {
		cout << "Gene out of bounds, quitting!" << endl;
		exit(1);
	}
	
	myGenes.at(geneNo) = newValue;
}

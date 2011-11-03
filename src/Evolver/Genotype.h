/*******************************************************************************
 * Genotype.h
 * 
 * Peter Fine - EASy MSC, Adaptive Systems Coursework
 * Created March '05
 * 
 ******************************************************************************/

#ifndef Genotype_h
#define Genotype_h

#include <vector>

// Forward Declarations
class Properties;

/**
 * Represents a genome and its error (to be minimized).
 */
class Genotype {

    public :

      /**
       * The array of genes.
       */
      std::vector<double> myGenes;
      
      /**
       * The fitness score.
       */
      double myFitness;
      
      /**
       * Whether it has been evaluated during the current generation.
       */
      bool myBeenEvaluated;
      
      /**
       * Whether this genotype has ever been constructed to create an agent. Useful for circumstances where the first
       * time an agent is constructed, the genome is altered (e.g. perhaps to make a CTRNN centre-crossing).
       */ 
      bool myHasEverBeenConstructed;
              
      /**
       * Create a new array of genes.
       */
       Genotype(int noGenes);

      /**
       * Create a Genotype as specified in a Properties object.
       */
       Genotype(Properties& theProperties);
       
       /**
        * Write the state of the genotype to a Properties object.
        */
       void write(Properties& theProperties);
        
      /**
       * A comparison function, for the sorting algorithm.
       */   
      static bool compare(const Genotype& a, const Genotype& b) {
        return a.myFitness > b.myFitness; 
      }
    
    	double getGene(int geneNo) { return myGenes.at(geneNo); }
    	void setGene(int geneNo, double newValue);
};

#endif


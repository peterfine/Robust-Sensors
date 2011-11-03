/*******************************************************************************
 * NeuralNetwork.h
 * 
 * Peter Fine - EASy MSC, Adaptive Systems Coursework
 * Created March '05
 * 
 ******************************************************************************/

#ifndef NeuralNetwork_h
#define NeuralNetwork_h

class Properties;
class Genotype;

#include <vector>
#include <iostream>

/**
 * An abstract NeuralNetwork, which interfaces between the simulator and neural
 * network code.
 */
class NeuralNetwork {
	protected :

		/**
		 * The number of inputs to the network.
		 */
		int myNoInputs;

		/**
		 * The number of outputs from the network.
		 */
		int myNoOutputs;	
	
		/**
		 * The inputs to the network.
		 */
		std::vector<double> myInputs;

		/**
		 * The outputs from the network.
		 */
		std::vector<double> myOutputs;
     
  public :
  
  	/**
  	 * Construct the neural network, with a given number of inputs and outputs.
  	 */
  	NeuralNetwork(int noInputs, int noOutputs) :
  		myNoInputs(noInputs), myNoOutputs(noOutputs),
  		myInputs(myNoInputs), myOutputs(myNoOutputs)
  		{ }
    
  	/**
     * Reconstruct the network, as specified by the provided genotype.
     * @param genotype The weights for the different connections.
     */
    virtual void reconstructNetwork(Genotype& genotype) = 0;
      
    /**
     * Update the network.
     * @param the time step (for integrated networks).
     */
    virtual void updateNetwork(double timestep) = 0;

    /**
     * Resets the network state.
     */
    virtual void resetNetwork() = 0;
      
    /**
     * Get integer network properties.
     * @param index The property index for the given property, defined in an
     * implementation.
     */
    virtual int getIntProperty(int index) { }

    /**
     * Get double network properties.
     * @param index The property index for the given property, defined in an
     * implementation.
     */
    virtual double getDoubleProperty(int index) { }

    /**
     * Set integer network properties.
     * @param index The property index for the given property, defined in an
     * implementation.
     * @param value The value to set this property to.
     */
    virtual void setIntProperty(int index, int value) { }

    /**
     * Set double network properties.
     * @param index The property index for the given property, defined in an
     * implementation.
     * @param value The value to set this property to.
     */
    virtual void setDoubleProperty(int index, double value) { }

		/**
		 * Set an input's value.
		 * @param inputNo The input number to set.
		 * @param value The value to set the input to.
		 */
		void setInput(int inputNo, double value) {
			myInputs.at(inputNo) = value; }

		/**
		 * Get an output's value.
		 * @outputNo The output number who's value should be returned.
		 */
		double getOutput(int outputNo) {	 
			return myOutputs.at(outputNo); }

		/**
		 * Calculate the number of genes required for this network.
		 */
		virtual int getNoGenes() = 0;
		
		static void addProperties(Properties& properties);
		
		virtual void resetRecords() = 0;
		
		virtual void updateRecords() = 0;
		
		virtual std::string getRecords(bool includeState) = 0;
		
    	virtual bool checkGenomeIsValid(Genotype& aGenotype) = 0;
};

#endif

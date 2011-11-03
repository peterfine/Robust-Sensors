/*******************************************************************************
 * NetworkFactory.h
 * 
 * Peter Fine - Thesis - Network Factory
 * Created May '05
 * 
 ******************************************************************************/

#ifndef _NETWORKFACTORY_H_
#define _NETWORKFACTORY_H_

class NeuralNetwork;
class CTRNNetwork;
class Properties;
class MTRand;

/**
 * Creates a NeuralNetwork object given a properties object.
 */
class NetworkFactory {
	
public:
	static NeuralNetwork* generateNetwork(Properties& theProperties,
																				MTRand& theRand);

	static void addProperties(Properties& theProperties);
};

#endif //_NETWORKFACTORY_H_

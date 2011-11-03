/*******************************************************************************
 * HomeostaticBraitenburgNetwork.h
 * 
 * Peter Fine - DPhil Dec '06
 ******************************************************************************/

#ifndef HomeostaticBraitenburgNetwork_h
#define HomeostaticBraitenburgNetwork_h

#include "NeuralNetwork.h"
#include <math.h>
#include <vector>
#include <iostream>

class Properties;
class Genotype;
class MTRand;

/**
* This class represents a homeostatic, Braitenburg style controller, as defined 
* in the second experiment of:
* Di Paolo, E. A., (2003). Organismically-inspired robotics: Homeostatic adaptation 
* and natural teleology beyond the closed sensorimotor loop, in: K. Murase & T. 
* Asakura (Eds) Dynamical Systems Approach to Embodiment and Sociality, Advanced 
* Knowledge International, Adelaide, Australia, pp 19 - 42.
* 
* Note it is not defined by a Genotype, and will quit the program if reconstruction
* is attempted!
*/
class HomeostaticBraitenburgNetwork : public NeuralNetwork {
	private :
	
		MTRand& myRand;

  	/**
  	 * The transfer function points.
  	 */
  	std::vector<double> myXPointsLeftMotor;
		std::vector<double> myYPointsLeftMotor;  
		std::vector<double> myXPointsRightMotor;
		std::vector<double> myYPointsRightMotor;
		
		// The SD of the random values added to the transfer function points.
		double myRandomisationSD;
		
		/**
		 * The battery properties.
		 */
		double myE;
		double myE1;
		double myMinE;
		double myMaxE;
		double myE2;
		double myInitialE;
		double myTauE;
		double mySensorInfluenceOnBattery;
		double test;
		double myMutationVariance;
		
		// The Records.
   	std::vector<std::vector<double> > myInputRecord;
  	std::vector<std::vector<double> > myOutputRecord;
  	std::vector<std::vector<double> >	myXPointsLeftMotorRecord;
		std::vector<std::vector<double> > myYPointsLeftMotorRecord;  
		std::vector<std::vector<double> > myXPointsRightMotorRecord;
		std::vector<std::vector<double> > myYPointsRightMotorRecord;
		std::vector<double> myERecord;
  	
 	public :

		/**
		 * Create a HomeostaticBraitenburgNetwork with the specified properties.
	 	 * @param properties The properties object used to configure this network.
		 */
		HomeostaticBraitenburgNetwork(Properties& properties, MTRand& rand);

		/**
		 * Update the network.
		 */
		void updateNetwork(double timeStep);

		/**
		 * Reset the network state.
		 */
		void resetNetwork();
		
    /**
     * Adds a definition of the properties required by this class to a 
     * Properties object.
     */
    static void addProperties(Properties& properties);

		/**
		 * Reconstruct network must be implemented (via NeuralNetwork), but is not
		 * appropriate here so will cause the program to quit.
		 */
		void reconstructNetwork(Genotype& genotype) {cout << "Can't Reconstruct HBN" << endl; exit(1); }
		
		int getNoGenes() {cout << "Can't getNoGenes" << endl; exit(1); }
		
		/**
		 * Fill a vector with random values between (0, 1), note exclusive boundaries.
		 */
		void randomizeVector(vector<double>& aVector);

		void mutateVector(double mutationMultiplier, vector<double>& aVector, int startIndex, int endIndex);

		// The Recorder functions.
    void resetRecords();
		void updateRecords();
		std::string getRecords(bool includeState);
		
		/**
		 * No genome constraints apply; the network is always valid.
		 */
    	bool checkGenomeIsValid(Genotype& aGenotype) {return true; }
};

#endif // HomeostaticBraitenburgNetwork_h

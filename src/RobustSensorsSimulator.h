
#ifndef Simulator_h
#define Simulator_h

#include "NeuralNetwork.h"

class Genotype;
class MTRand;
class Properties;

#include<vector>

/**
 * Sets up the simulation and, using a GeneticAlgorithm, controls the 
 * experiments that is runs.
 */
class RobustSensorsSimulator {
		public :
			
			/**
			 * The neural network which controls the activity of this simulation.
			 */
			NeuralNetwork& myNetwork;
			
			/**
			 * The Properties.
			 */
			Properties& myProperties;
			
			/**
			 * The Random number generator.
			 */
			MTRand& myRand;
			
			/**
			 * The number of lights to display per trial.
			 */
			int myNoLights;
			
			bool myOverwriteLightAngle;
			
			/**
			 * The length of time prior to each change of light during which the fitness is recorded.
			 * This is to allow the agent to settle down and find the light before being punished for being
			 * distant from it.
			 */ 
			double myFitnessDuration;
			
			double myTimestep;

			/**
			 * The amount of time passed.
			 */
			double myTime;
			
			double mySensor1Angle;

			/**
			 * The agent's radius.
			 */
			double myRadius;
			double myLightDurationMin;
			double myLightDurationMax;
			double myLightDistanceMin;
			double myLightDistanceMax;
			double myTimeUntilNextLight;
			double myAgentX;
			double myAgentY;
			double myAgentHeading;
			double myLightX;
			double myLightY;
			double myPi;
			double mySensor1X;
			double mySensor1Y;
			double mySensor1Input;
			
			bool myLightContributesToFitness;
			bool myChangeAngleDuringTrial;
			vector<int> myChangeAngleSchedule;
			bool mySensorAngle1D;
			
			bool myExperimentHasEnded;
			
			int myLightNo;
			double myUnNormalizedFitness;
			double myTotalFitnessTime;
			 
    	/**
    	 * Create a RobustSensorsSimulator.
    	 */
    	RobustSensorsSimulator(NeuralNetwork& network, Properties& properties, 
    												 MTRand& random);
    	
    	/**
    	 * Configure a simulation.
    	 */
    	void configure();
    	
    	void overwriteSensorAngle(double angle1);
    	
    	void setChangeAngleSchedule(vector<int>& aChangeAngleSchedule);
    		
    	void generateChangeAngleSchedule(int noChanges);
    	
    	void generateSingleChangeAngleSchedule(int meanLightNo, int maxVariationFromMean);
    	
    	/**
    	 * Construct the controller from a genotype.
    	 */
    	void reconstruct(Genotype& genotype);
    	
    	/**
    	 * Run a simulation, returning the fitness score.
    	 */
    	double runSimulation();

    	/**
    	 * Advance the simulation by one timestep.
    	 */
    	void update();
    	
      /**
       * Adds a definition of the properties required by this class to a 
       * Properties object.
       */
      static void addProperties(Properties& properties); 
      
		private :
		
			/**
			 * Chose a random light location.
			 */
			void resetLight(bool lightContributesToFitness);
			
			/**
			 * Determine whether to change the angle, and do so if required.
			 * @returns whether the angle was changed or not.
			 */
			bool changeAngleIfRequired();
};

#endif // Simulator_h

/*******************************************************************************
 * RobustSensorsManagerNonEvo.h
 * 
 * Peter Fine - EASy MSC, Thesis - Active Vision sensor.
 * Created March '05
 * 
 ******************************************************************************/

#ifndef RobustSensorsManagerNonEvo_h
#define RobustSensorsManagerNonEvo_h

#include "MersenneTwister.cpp"
#include "Properties.h"
#include "NeuralNetwork.h"
#include "RobustSensorsSimulator.h"
#include "RobustSensorsRecorder.h"

/**
 * Run a homeostatic (non-evolutionary) simulation, outputting results to a 
 * file for visulisation.
 */
class RobustSensorsManagerNonEvo {
	private:
	
		/**
		 * The Properties object containing the definition of the agent/simuation
		 * to be recorded.
		 */
		Properties mySimulationProperties;
		
		/**
		 * The random number generator used by the simulation.
		 */
		MTRand myRand;

		/**
		 * The Simulation.
		 */
		RobustSensorsSimulator mySim;
		
		/**
		 * The Recorder.
		 */
		RobustSensorsRecorder myRecorder;
		
	public:
	
		/**
     * Create an RobustSensors Manager.
     */
    RobustSensorsManagerNonEvo(Properties& simulationProperties);
    
		/**
		 * Configure and run a simulation.
		 */
		double runSimulation();
    
    static void addProperties(Properties& properties);
};

#endif // RobustSensorsManagerNonEvo_h

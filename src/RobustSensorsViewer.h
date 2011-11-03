/*******************************************************************************
 * XViewer.h
 * 
 * Peter Fine - EASy MSC, Thesis - Active Vision sensor.
 * Created March '05
 * 
 ******************************************************************************/

#ifndef RobustSensorsViewer_h
#define RobustSensorsViewer_h

#include "MersenneTwister.cpp"
#include "Properties.h"
#include "NeuralNetwork.h"
#include "RobustSensorsSimulator.h"
#include "RobustSensorsRecorder.h"

/**
 * Load a genotype from a file, and run a simulation, outputting results to a 
 * file for visulisation.
 */
class RobustSensorsViewer {
	private:
	
		/**
		 * The Properties for the viewer.
		 */
		Properties& myViewerProperties;
	
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
		RobustSensorsSimulator* mySim;
		
		/**
		 * The Recorder.
		 */
		RobustSensorsRecorder* myRecorder;
		
		// Roll these into some kind of misc utils class later.
		vector<string> tokenize(const string& str, const string& delimiters);
		vector<int> convertStringVecToIntVec(vector<string>& strings);
		
	public:
		/**
     * Create an RobustSensors viewer.
     */
    RobustSensorsViewer(Properties& viewerProps);
    
    /**
     * Run the viewer, generating output.
     */
    void runViewer();
    
		/**
		 * Run a simulation, as configured by runViewer.
		 */
		double runSimulation();
    
    static void addProperties(Properties& properties);
};

#endif // RobustSensorsViewer_h

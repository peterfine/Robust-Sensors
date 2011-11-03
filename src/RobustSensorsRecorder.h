/*******************************************************************************
 * RobustSensorsRecorder.h
 * 
 * Peter Fine - DPhil - Holds records of the state and properties of a Simulation.
 * Created Dec '06
 * 
 ******************************************************************************/
 
#ifndef RobustSensorsRecorder_h
#define RobustSensorsRecorder_h

#include <vector>
#include <string>

class RobustSensorsSimulator;
class Properties;

class RobustSensorsRecorder {
	
	public :

		RobustSensorsRecorder(RobustSensorsSimulator& sim, Properties& recorderProperties);	
		void resetRecords();
		void updateRecords();
		std::string printRecords();
		void addViewerSpecificData(std::string& records, Properties& viewerProperties, double fitness, std::vector<double> lightChangeTimes);
		void addNonEvoSpecificData(std::string& records, Properties& simulationProperties);
		
		static void addProperties(Properties& properties);
		
	private :
	
		// The Simulator.
		RobustSensorsSimulator& mySim;
		int myFrequency;
		int myCallsTillNextUpdate;
		/**
		 * Whether to record the state each myFrequency, or not.
		 */
		bool myRecordSimulationData;
					
		// The records.
		std::vector<double> myRecAgentX;
		std::vector<double> myRecAgentY;
		std::vector<double> myRecLightX;
		std::vector<double> myRecLightY;
		std::vector<double> myRecLightNo;
		std::vector<double> myRecSensor1X;
		std::vector<double> myRecSensor1Y;
		std::vector<double> myRecAgentHeading;
		std::vector<double> myRecTime;
		std::vector<double> myRecTimeUntilNextLight;
		std::vector<double> myRecUnNormalizedFitness;
		std::vector<double> myRecSensor1Input;
		std::vector<double> myRecSensor1Angle;
		std::vector<bool>   myRecLightContributesToFitness;
};

#endif // RobustSensorsRecorder_h

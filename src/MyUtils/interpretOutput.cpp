#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

int main(int argc, char** argv) {

	if(argc != 3) {
		cout << "usage: Interpret dirPath targetPath" << endl;
		exit(1);
	}

	// Create a list of the files in the target directory.
	stringstream command;
	command << "cd " << argv[1] << " ; ls *Record*.* > filelist.tmp";
	//command << "ls " << argv[1] << "/*Record.* > filelist.tmp";
  system(command.str().c_str());

	stringstream fileListSS;
	fileListSS << argv[1] << "/filelist.tmp";  
  fstream fileList(fileListSS.str().c_str());
  
  vector<string> theFiles;
  vector<string> theFileNames;
  string fileName;
  while(fileList >> fileName) {
  	stringstream fullFileName;
  	fullFileName << argv[1] << fileName;
  	theFiles.push_back(fullFileName.str());
  	theFileNames.push_back(fileName);
  }
	stringstream removeCommand;
	removeCommand << "rm " << argv[1] << "/filelist.tmp";
	system(removeCommand.str().c_str());
   
	// Process the contents of those files.
	vector< vector<string> > bestFitnessRecord;
	vector< vector<string> > averageFitnessRecord;
	
	for(int file = 0; file < theFiles.size(); file++) {
		// Add headers to the listings.
		vector<string> bestFitness;
		vector<string> averageFitness;
		
		bestFitness.push_back(theFileNames.at(file));
		averageFitness.push_back(theFileNames.at(file));
		
		// Open the file and extract a vector of their best and average fitnesses.
		fstream results(theFiles.at(file).c_str());
		string line;
		bool firstLine = true;
		while(getline(results, line)) {
			
			// Ignore the first line.
			if(!firstLine) {
				// read the results.
				stringstream theLine(line);
	
				// ignore the first item (the generation number).
				string ignored;
				string best;
				string average;

				// Record these values.
				theLine >> ignored >> best >> average;
				bestFitness.push_back(best);
				averageFitness.push_back(average);
			}
				else {
					firstLine = false;
				}
		}
		results.close();
		
		// Add the results to the records.
		bestFitnessRecord.push_back(bestFitness);
		averageFitnessRecord.push_back(averageFitness);
	}
	
	// Create a new file for the best and aveage listings.
	stringstream bestFileName;
	stringstream averageFileName;
	bestFileName << argv[2] << "Best.m";
	averageFileName  << argv[2] << "Average.m";
	
	ofstream best;
	ofstream average;
	
	best.open(bestFileName.str().c_str(), ios::trunc);
	average.open(averageFileName.str().c_str(), ios::trunc);
	
	// MATLAB VERSION
	best << "clear title; clear fitness;" << endl;
	average << "clear title; clear fitness;" << endl;
	for(int exp = 0; exp < bestFitnessRecord.size(); exp++) {
		best << "expName{" << exp+1 << "} = '" << bestFitnessRecord.at(exp).at(0) << "';" << endl 
				 << "fitness{" << exp+1 << "} = ["; 
		average << "expName{" << exp+1 << "} = '" << averageFitnessRecord.at(exp).at(0) << "';" << endl
						<< "fitness{" << exp+1 << "} = [";
		
		for(int gen = 1; gen < bestFitnessRecord.at(exp).size(); gen++) {
			best << bestFitnessRecord.at(exp).at(gen) << " ";
			average << averageFitnessRecord.at(exp).at(gen) << " ";
		}
		best << "];" << endl;
		average << "];" << endl;
	}
			
	
	// NEW VERSION, 1 EXP Per ROW
/*
	for(int exp = 0; exp < bestFitnessRecord.size(); exp++) {
		for(int gen = 0; gen < bestFitnessRecord.at(exp).size(); gen++) {
			best << bestFitnessRecord.at(exp).at(gen);
			average << averageFitnessRecord.at(exp).at(gen);
			if( gen != (bestFitnessRecord.at(exp).size() - 1)) {
				best << " ";
				average << " ";
			}
		}
		
		best << "\n";
		average << "\n";
	}
	*/
/*	OLD VERSION, 1 EXP PER COLUMN
	
	bool stillContainsData;
	int gen = 0;
	do {
		stillContainsData = false;
		stringstream bestLine;
		stringstream averageLine;
		for(int rec = 0; rec < bestFitnessRecord.size(); rec++) {
			
			string suffix = " ";
			if(rec == bestFitnessRecord.size() - 1) {
				suffix = "";
			}
			
			if(gen < bestFitnessRecord.at(rec).size()) {
				// append the files with the current information.
				bestLine << bestFitnessRecord.at(rec).at(gen) << suffix;
				averageLine << averageFitnessRecord.at(rec).at(gen) << suffix;
				stillContainsData = true;				
			}
				else {
					bestLine << suffix;
					averageLine << suffix;
				}
		}
		if(stillContainsData) {
			best << bestLine.str() << "\n";
			average << averageLine.str() << "\n";
		}

		gen++;
	} while(stillContainsData);
	
*/

	best.close();
	average.close();
	
}

/*******************************************************************************
 * Recorder.cpp
 * 
 * Peter Fine - EASy MSC, Adaptive Systems Coursework
 * Created March '05
 * 
 ******************************************************************************/

using namespace std;

#include "Recorder.h"
#include "Properties.h"

#include <sstream>
#include <fstream>
#include <istream>
#include <iostream>

void Recorder::updateRecorder(const string recordUpdate) {
	myRecord += recordUpdate;
}

void Recorder::updateRecorder(Properties& properties) {
 	stringstream propertyStream;
 	// Serialize the Properties object, using its own operators.
	propertyStream << properties;
	
	// Overwrite the record with the new properties.
	myRecord = propertyStream.str();
}

void Recorder::writeRecord() {
	ofstream file;
	
	if(myReplaceFile) {
		// Replace the file.
		file.open(myRecordPath.c_str(), ios::trunc);
	}
		else {
			// Append the file.
			file.open(myRecordPath.c_str(), ios::app);
		}
	
	file.precision(100);
  file << myRecord;
  file.close();
  
  // And delete the written record.
  myRecord = "";
}

void Recorder::loadRecord(string filePath, Properties& properties) {
// Open the specified file and read the data.
  ifstream aFile;
  aFile.open(filePath.c_str());
  aFile.precision(100);
  
  // Check that this suceeded.
  if(aFile.is_open() == false) {
    cout << "File not found, quitting." << endl;
    exit(1);
  }

  // De-serialize the Properties object.
  if(!(aFile >> properties)) {
  	cout << "De-serialization failed, quitting." << endl;
  	exit(1);
  }
  aFile.close();
}


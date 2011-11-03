/*******************************************************************************
 * Properties.cpp
 * 
 * Peter Fine - Programming Utils - Command Line Parser
 * Created March '05
 * 
 ******************************************************************************/


#include <sstream>

#include "Properties.h"

#include <fstream>

Properties::Properties(string filename) {
	fstream propertyFile(filename.c_str());
	propertyFile >> *this;
}

ostream& operator<<(ostream& output, const Properties& aProperties) {

	// Iterate through the items and add them to the output stream.
	map<string, Item>::const_iterator itemIter;
	for(itemIter = aProperties.myMap.begin(); itemIter != aProperties.myMap.end(); 
			itemIter++) {
		string nextItemLabel = (*itemIter).first;
		Item nextItem = (*itemIter).second;
		// Write the label, then the corresponding Item object.
		output << nextItemLabel << " " << nextItem;
	}
  
  return output;
}

istream& operator>>(istream& input, Properties& aProperties) {
  // Each line contains a label and an item.
 string newItemName;
 Item newItem;
  while(input >> newItemName >> newItem) {

	  // Add the reconstructed item to the map. 
	  // This will replace items of the same name.
	  aProperties.myMap[newItemName] = newItem;
  }
}

void Properties::addIntItem(string itemName) {
	Item tempItem;
	tempItem.itemTypeIndex = 1;
	myMap[itemName] = tempItem;
}


void Properties::addIntItem(string itemName, int value) {
	// The item to be copied into the map.
	Item tempItem;
	tempItem.itemTypeIndex = 1;
	tempItem.intValue = value;
	tempItem.hasBeenDefined = true;
	myMap[itemName] = tempItem;
}

void Properties::addDoubleItem(string itemName) {
	Item tempItem;
	tempItem.itemTypeIndex = 2;
	myMap[itemName] = tempItem;
}


void Properties::addDoubleItem(string itemName, double value) {
	Item tempItem;
	tempItem.itemTypeIndex = 2;
	tempItem.doubleValue = value;
	tempItem.hasBeenDefined = true;	
	myMap[itemName] = tempItem;
}

void Properties::addStringItem(string itemName) {
	Item tempItem;
	tempItem.itemTypeIndex = 3;
	myMap[itemName] = tempItem;
}

void Properties::addStringItem(string itemName, string value) {
	Item tempItem;
	tempItem.itemTypeIndex = 3;
	tempItem.stringValue = value;
	tempItem.hasBeenDefined = true;	
	myMap[itemName] = tempItem;
}

void Properties::addBoolItem(string itemName) {
	Item tempItem;
	tempItem.itemTypeIndex = 4;
	myMap[itemName] = tempItem;
}

void Properties::addBoolItem(string itemName, bool value) {
	Item tempItem;
	tempItem.itemTypeIndex = 4;
	tempItem.boolValue = value;
	tempItem.hasBeenDefined = true;	
	myMap[itemName] = tempItem;
}

void Properties::parseCommandLine(int argc, char** argv) {
	// Put the commnd line arguments into a vector for processing.
	vector<string> properties;
	for(int i = 1; i < argc; i++) {
		properties.push_back(string(argv[i]));
	}
	
	// Run through the parser.
	parseProperties(properties, true);
}

void Properties::parseProperties(const vector<string>& properties, 
																bool requireAll) {
	
	// Check there are pairs of argument label and data.
  if((properties.size() % 2) != 0) {
    cout << properties.size() << " is an odd number of arguments!" << endl; 
    exit(1);
  }
  
  // Process the properties.
  for(int i = 0; i < properties.size(); i += 2) {
    
    // Check if the item exists.
    if(myMap.count(properties[i]) == 1) {

			myMap[properties[i]].hasBeenDefined = true;

			// Create a stream to allow the conversion of the value following this 
			// label.
			stringstream valueStream(properties[i + 1]);
			valueStream.precision(30);

    	// Add the defined value to the map.
    	// The value is an int.
    	if(myMap[properties[i]].itemTypeIndex == 1) {
    		valueStream >> myMap[properties[i]].intValue;
    	}
    		else if(myMap[properties[i]].itemTypeIndex == 2) {
    			valueStream >> myMap[properties[i]].doubleValue;
    		}
    		else if(myMap[properties[i]].itemTypeIndex == 3) {
    			valueStream >> myMap[properties[i]].stringValue;
    		}
    		else if(myMap[properties[i]].itemTypeIndex == 4) {
    			valueStream >> myMap[properties[i]].boolValue;
    		}
    		else {
    			// Not a valid property type.
    			cout << myMap[properties[i]].itemTypeIndex 
    					 << " is an invalid itemType" << endl;
    			exit(1);
    		}
    }
    	else {
	  		// The item was not a valid command, so quit.
	      cout << properties[i] << " not recognized, quitting." << endl;
  	    exit(1);
    	}
  }
  
  //Check that all the properties have been defined, if requested.
  if(requireAll) {
		map<string, Item>::iterator itemIter;
		bool allDefined = true;
		for(itemIter = myMap.begin(); itemIter != myMap.end(); itemIter++)
		{
			Item nextItem = (*itemIter).second;
			if(!nextItem.hasBeenDefined) {
				// The item has not been defined.
				if(!allDefined) {
					cout << ", ";
				}
				cout << (*itemIter).first;
				allDefined = false;
			}
		}
		
		if(!allDefined) {
			cout << " not provided, quitting." << endl;
			exit(1);
		}
  }
}
  
int Properties::getInt(string itemName) {
	
	if(myMap.count(itemName) == 1) {
		if(myMap[itemName].itemTypeIndex == 1) {
			if(myMap[itemName].hasBeenDefined == true) {
				return myMap[itemName].intValue;
			}
				else {
					cout << itemName << " had not been defined, quitting." << endl;
					exit(1);					
				}
		}
			else {
				cout << itemName << " is not an integer, quitting." << endl;
				exit(1);
			}
	}
		else {
				cout << itemName << " was not found, quitting." << endl;
				exit(1);
		}
}
	
double Properties::getDouble(string itemName) {
	if(myMap.count(itemName) == 1) {
		if(myMap[itemName].itemTypeIndex == 2) {
			if(myMap[itemName].hasBeenDefined == true) {			
				return myMap[itemName].doubleValue;
			}
				else {
					cout << itemName << " had not been defined, quitting." << endl;
					exit(1);					
				}			
		}
			else {
				cout << itemName << " is not a double, quitting." << endl;
				exit(1);
			}
	}
		else {
				cout << itemName << " was not found, quitting." << endl;
				exit(1);
		}

}

string Properties::getString(string itemName) {

	if(myMap.count(itemName) == 1) {
		if(myMap[itemName].itemTypeIndex == 3) {			
			if(myMap[itemName].hasBeenDefined == true) {
				return myMap[itemName].stringValue;
			}
				else {
					cout << itemName << " had not been defined, quitting." << endl;
					exit(1);					
				}			
		}
			else {
				cout << itemName << " is not a string, quitting." << endl;
				exit(1);
			}
	}
		else {
				cout << itemName << " was not found, quitting." << endl;
				exit(1);
		}

}

bool Properties::getBool(string itemName) {
	if(myMap.count(itemName) == 1) {
		if(myMap[itemName].itemTypeIndex == 4) {
			if(myMap[itemName].hasBeenDefined == true) {			
				return myMap[itemName].boolValue;
			}
				else {
					cout << itemName << " had not been defined, quitting." << endl;
					exit(1);					
				}			
		}
			else {
				cout << itemName << " is not a bool, quitting." << endl;
				exit(1);
			}
	}
		else {
				cout << itemName << " was not found, quitting." << endl;
				exit(1);
		}
}


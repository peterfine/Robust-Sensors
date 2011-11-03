/*******************************************************************************
 * Properties.h
 * 
 * Peter Fine - Programming Utils - Properties parser/holder/saver
 * Created March '05
 * 
 ******************************************************************************/

#ifndef _Properties_H_
#define _Properties_H_

using namespace std;

#include <iostream>
#include <map>
#include <vector>

#include "Item.h"

class Properties
{
   friend ostream& operator<<(ostream& output, const Properties& aProperties);
   friend istream& operator>>(istream& input, Properties& aProperties);	
	
public:
	/**
	 * A map containing the property values and their names.
	 */
	map<string, Item> myMap;
	
	/**
	 * Parse a set of properties, used by the public methods.
	 */
	void parseProperties(const vector<string>& properties, bool requireAll);
		
public:
	
	/**
	 * Default constructor, creating an empty Properties object.
	 */
	Properties() { }
	
	/**
	 * Constructor, reconstructing a Properties object from a file.
	 */
	Properties(string filename);
	
	/**
	 * Parse the supplied command line arguments.
	 * @param argc The number of items on the command line.
	 * @param argv The command line preferences to parse.	 * 
	 */	
	void parseCommandLine(int argc, char** argv);
	
	/**
	 * Add an integer item which will be present on the command line.
	 * @param name The name of the item to add.
	 */
	void addIntItem(string itemName);

	void addIntItem(string itemName, int value);

	/**
	 * Add a double item which will be present on the command line.
	 * @param name The name of the item to add.
	 */
	void addDoubleItem(string itemName);

	void addDoubleItem(string itemName, double value);

	/**
	 * Add a string (or char*) item which will be present on the command line.
	 * @param name The name of the item to add.
	 */
	void addStringItem(string itemName);

	void addStringItem(string itemName, string value);

	/**
	 * Add a bool item which will be present on the command line.
	 * @param name The name of the item to add.
	 */
	void addBoolItem(string itemName);
	
	void addBoolItem(string itemName, bool value);


	
	int 	 getInt(string itemName);
	double getDouble(string itemName);
	string getString(string itemName);
	bool   getBool(string itemName);
};

#endif //_Properties_H_

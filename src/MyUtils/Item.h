/*******************************************************************************
 * Item.h
 * 
 * Peter Fine - Programming Utils - Command Line Parser
 * Created March '05
 * 
 ******************************************************************************/
 
#ifndef _Item_H_
#define _Item_H_

using namespace std; 

#include <iostream>
 
/**
 * A simple class allowing different data types to be held by a single object.
 * Supports io using streams.
 */
class Item {
   friend ostream& operator<<(ostream& output, const Item& anItem);
   friend istream& operator>>(istream& input, Item& anItem);
	
	public :
  	int itemTypeIndex; // An index to the data type of this item.
		bool hasBeenDefined; // Whether the value has been set.
	
		int intValue;
		double doubleValue;
		string stringValue;
		bool boolValue;
		
		Item() : itemTypeIndex(0), intValue(0), doubleValue(0.0), stringValue(""), 
   					 boolValue(false), hasBeenDefined(false) {}		
};

#endif //_Item_H_

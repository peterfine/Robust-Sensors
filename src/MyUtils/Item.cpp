/*******************************************************************************
 * Item.cpp
 * 
 * Peter Fine - Programming Utils - Command Line Parser
 * Created March '05
 * 
 ******************************************************************************/
 
#include "Item.h" 
 
ostream& operator<<(ostream& output, const Item& anItem) {

	output.precision(30);
	
  output << anItem.itemTypeIndex << " " << anItem.hasBeenDefined << " ";
  switch(anItem.itemTypeIndex) {
  	case 1:
  	  output << anItem.intValue;
  	break;   
  	case 2:
  	  output << anItem.doubleValue;
  	break;   
  	case 3:
  	  output << anItem.stringValue;
  	break;
  	case 4:
  	  output << anItem.boolValue;
  	break;
  }
   
  output << endl;
  
  return output;
}

istream& operator>>(istream& input, Item& anItem) {

  input >> anItem.itemTypeIndex >> anItem.hasBeenDefined;
  switch(anItem.itemTypeIndex) {
  	case 1:
  	  input >> anItem.intValue;
  	break;   
  	case 2:
  	  input >> anItem.doubleValue;
  	break;   
  	case 3:
  	  input >> anItem.stringValue;
  	break;
  	case 4:
  	  input >> anItem.boolValue;
  	break;
  }
   
  return input;
}

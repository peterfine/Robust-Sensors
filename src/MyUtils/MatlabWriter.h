/*******************************************************************************
 * MatlabWriter.h
 * 
 * Peter Fine - Matlab Writer.
 * Created Oct '06
 * 
 ******************************************************************************/
 
#ifndef _MatlabWriter_H_
#define _MatlabWriter_H_

using namespace std; 

#include <iostream>
#include <vector>
 
/**
 * Writes standard datatypes as a matlab script.
 */
class MatlabWriter {
	
	public :
		static string myExtraPrefix;
	
  	static string parse(const bool& value, const string& variableName);
  	
  	static string parse(const vector<bool>& value, const string& variableName);

  	static string parse(const double& value, const string& variableName);
  	
  	static string parse(const vector<double>& value, const string& variableName);
  	
  	static string parse(const int& value, const string& variableName);
  	
  	static string parse(const vector<int>& value, const string& variableName);
  	
  	static string parse(const string& value, const string& variableName);
  	
  	static string parse(const vector<string>& value, const string& variableName);
  	
  	static string parse(const vector<vector<double> >& value, const string& variableName);
  	
  	static string parse(const vector<vector<int> >& value, const string& variableName);
  	
  	static string parse(const vector<vector<string> >& value, const string& variableName);
};

#endif //_MatlabWriter_H_

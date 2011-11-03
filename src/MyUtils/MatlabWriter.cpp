/*******************************************************************************
 * MatlabWriter.cpp
 * 
 * Peter Fine - Matlab Writer
 * Created Oct '06
 * 
 ******************************************************************************/
 
#include "MatlabWriter.h"

#include<sstream> 

string MatlabWriter::myExtraPrefix = ""; // Default extra prefix is empty.

string MatlabWriter::parse(const bool& value, const string& variableName) {
	string fullName = myExtraPrefix + variableName;
	stringstream result;
	result << fullName << " = " << value << "; " << endl;
	return result.str();
}
  	
string MatlabWriter::parse(const vector<bool>& value, const string& variableName) {
	string fullName = myExtraPrefix + variableName;
	stringstream result;
	result << fullName << " = [";
	for(int i = 0; i < value.size(); i++) {
		result << value.at(i) << " ";
	}
	result << "];" << endl;
	
	return result.str();
}

string MatlabWriter::parse(const double& value, const string& variableName) {
	string fullName = myExtraPrefix + variableName;
	stringstream result;
	result << fullName << " = " << value << "; " << endl;
	return result.str();
}
  	
string MatlabWriter::parse(const vector<double>& value, const string& variableName) {
	string fullName = myExtraPrefix + variableName;
	stringstream result;
	result << fullName << " = [";
	for(int i = 0; i < value.size(); i++) {
		result << value.at(i) << " ";
	}
	result << "];" << endl;
	
	return result.str();
}
  	
string MatlabWriter::parse(const int& value, const string& variableName) {
	string fullName = myExtraPrefix + variableName;
	stringstream result;
	result << fullName << " = " << value << "; " << endl;
	return result.str();
}
  	
string MatlabWriter::parse(const vector<int>& value, const string& variableName) {
	string fullName = myExtraPrefix + variableName;
	stringstream result;
	result << fullName << " = [";
	for(int i = 0; i < value.size(); i++) {
		result << value.at(i) << " ";
	}
	result << "];" << endl;
	
	return result.str();
}
  	
string MatlabWriter::parse(const string& value, const string& variableName) {
	string fullName = myExtraPrefix + variableName;
	stringstream result;
	result << fullName << " = '" << value << "'; " << endl;
	return result.str();
}
  	
string MatlabWriter::parse(const vector<string>& value, const string& variableName) {
	string fullName = myExtraPrefix + variableName;
	stringstream result;
	result << fullName << " = [";
	for(int i = 0; i < value.size(); i++) {
		result << "'" << value.at(i) << "' ";
	}
	result << "];" << endl;
	
	return result.str();
}

string MatlabWriter::parse(const vector<vector<double> >& value, const string& variableName) {
	string fullName = myExtraPrefix + variableName;
	stringstream result;
	result << fullName << " = [";
	for(int vec = 0; vec < value.size(); vec++) {
		for(int i = 0; i < value[vec].size(); i++) {
			result << value.at(vec).at(i) << " ";
		}
		result << ";";
	}
	result << "];" << endl;
	
	return result.str();
}
  	
string MatlabWriter::parse(const vector<vector<int> >& value, const string& variableName) {
	string fullName = myExtraPrefix + variableName;
	stringstream result;
	result << fullName << " = [";
	for(int vec = 0; vec < value.size(); vec++) {
		for(int i = 0; i < value[vec].size(); i++) {
			result << value.at(vec).at(i) << " ";
		}
		result << ";";
	}
	result << "];" << endl;
	
	return result.str();
}

string MatlabWriter::parse(const vector<vector<string> >& value, const string& variableName) {
	string fullName = myExtraPrefix + variableName;
	stringstream result;
	result << fullName << " = [";
	for(int vec = 0; vec < value.size(); vec++) {
		for(int i = 0; i < value.at(vec).size(); i++) {
			result << " '" << value.at(vec).at(i) << "' ";
		}
		result << ";";
	}
	result << "];" << endl;
	
	return result.str();
}

using namespace std;

#include <iostream>
#include <sstream>
#include "Properties.h"
#include "Recorder.h"

int main(int argc, char** argv) {
  
	Properties parser;
/*
	parser.addIntItem("int34", 5);
	parser.addIntItem("blahinteger", 56665);
	parser.addStringItem("test", "tester_tester_t");
	parser.addDoubleItem("tht", 2445.345345345);
	parser.addBoolItem("booleaner", false);

	Recorder rec("../out.txt", true);
	rec.updateRecorder(parser);
	rec.writeRecord();
*/
	Properties parser2;
  Recorder::loadRecord("../out.txt", parser2);
  cout << parser2;

	
	char t;
	cin >> t;

}

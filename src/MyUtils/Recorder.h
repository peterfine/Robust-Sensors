/*******************************************************************************
 * Recorder.h
 * 
 * Peter Fine - EASy MSC, Adaptive Systems Coursework
 * Created March '05
 * 
 ******************************************************************************/

#ifndef Recorder_h
#define Recorder_h

using namespace std;

#include <string>

class Properties;

/**
 * Stores information about the state of an experiment, and writes to record 
 * files.
 */
class Recorder {
  private :
    /**
     * The current record to be printed to a file.
     */
    string myRecord;
    
    /**
     * The path to the file to be recorder.
     */
    string myRecordPath;

		/**
		 * Whether to replace or append the file when new data is written.
		 */
		bool myReplaceFile;
		

  public :
    /**
     * Create a record.
     * @param filepath The path for the record.
     * @param replaceFile Whether to replace the file or not.
     */
    Recorder(string filePath, bool replaceFile) : 
      myRecordPath(filePath), myReplaceFile(replaceFile) { }
  
    /**
     * Update the recorder with a string.
     */
    void updateRecorder(const string recordUpdate);
    
    /**
     * Update the recorder with a Properties object.
     */
    void updateRecorder(Properties& properties);
    
    /**
     * Write the current record to a file.
     */
    void writeRecord();
    
    /**
     * Loads a Properties object from a file.
     * @param filePath The file containing the serialized Properties object.
     * @param properties A reference to a Properties object to update.
     */
    static void loadRecord(string filePath, Properties& properties);
    
    string getPath() { return myRecordPath; }
  
};

#endif

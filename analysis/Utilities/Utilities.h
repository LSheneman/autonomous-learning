/*
 * Utilities.h
 *
 *  Created on: Oct 6, 2015
 *      Author: cliff
 */

#ifndef __BasicMarkovBrainTemplate__Utilities__
#define __BasicMarkovBrainTemplate__Utilities__

#include <algorithm>
#include <sstream>

using namespace std;

/*
 * return x % y were (-1 % y) = (y - 1)
 * (-2 % y) = (y - 2), etc.
 */
inline int loopMod(const int numerator, const int denominator) {
  return ((numerator % denominator) + denominator) % denominator;
}
/*
 * returns 1 if "d" is greater than 0, else return 0
 */
inline int Bit(double d) {
  return (d > 0.0);
}

inline vector<string> parseCSVLine(string rawLine, const char separator = ',') {
  vector<string> dataLine;
  string dataEntry;
  bool inQuote = false;

  dataEntry.clear();
  for (auto c : rawLine) {
    if (inQuote) {  // if inQuote
      if (c == '\"') {  // found the closed quote, we are at the end of the quote
        inQuote = false;
      } else {
        dataEntry.push_back(c);  // else just append c to the back of dataEntry
      }
    } else {  // if not in quote
      if (!isblank(c)) {
        if (c == '\"') {  // we have just entered some quoted text, just read everything until the next quote
          inQuote = true;
        } else {  // we are not inQuote and this c is not whitespace
          if (c == separator) {  // if we are at a separator
            dataLine.push_back(dataEntry);  // add dataEntry to dataLine
            dataEntry.clear();  // ... and clear out dataEntry to make room for the next value
          } else {
            dataEntry.push_back(c);  // look it's just text, add it to dataEntry
          }
        }

        // read non " non whitespace stuff (i.e. look for separators and entries);

      } else {  // it is whitespace. there should be no whitespace in this file. Error.
        cout << "In FileManager::parseCSVLine(string rawLine)\nWhile attempting to read csv file, encountered whitespace. This file should have no whitespace. Exiting.\n";
        exit(1);
      }
    }
  }
  dataLine.push_back(dataEntry);  // we are at the end of the line. add dataEntry to dataLine
  dataEntry.clear();  // ... and clear out dataEntry to make room for the next value
  return dataLine;
}

// reads a csv file with header and converts to a map of string,vector<string>
inline map<string, vector<string>> readFromCSVFile(const string& fileName, const char separator = ',') {
  std::ifstream FILE(fileName);
  map<string, vector<string>> data;  // the final map
  vector<string> dataLine;  // one line of data, converted from a single csv string to a vector of strings
  string rawLine;
  bool firstLine = true;
  vector<string> lookUpTable;

  if (FILE.is_open())  // if the file named by configFileName can be opened
  {
    dataLine.clear();
    while (getline(FILE, rawLine))  // keep loading one line from the file at a time into "line" until we get to the end of the file
    {
      dataLine = parseCSVLine(rawLine, separator);
      if (firstLine) {  // this is the first line, dataLine contains the keys... use them to build the map and loopUpTable
        for (auto key : dataLine) {
          lookUpTable.push_back(key);  // add the key so we can make sure we assign the right values to the right columns
          data[key] = {};  // add an empty vector for each key into the map data
        }
        firstLine = false;
      } else {  // we are not in the first line, dataLine has values
        for (size_t i = 0; i < dataLine.size(); i++) {  // for each entry in dataLine
          data[lookUpTable[i]].push_back(dataLine[i]);
        }
      }
    }
  }
  return data;
}

// Put an arbitrary value to the target variable, return false on conversion failure
template<class T>
static bool load_value(const string& value, T& target) {
  std::stringstream ss(value);
  ss >> target;
  if (ss.fail()) {
    return false;
  } else {
    string remaining;
    ss >> remaining;
    // stream failure means nothing left in stream, which is what we want
    return ss.fail();
  }
}

// converts a vector of strings to a vector of type of returnData
template<class T>
void convertCSVListToVector(string stringData, vector<T> &returnData, const char separator = ',') {
  returnData.clear();
  stringData = stringData.substr(1, stringData.size() - 2); // strip off leading and trailing square brakets
  vector<string> dataLine = parseCSVLine(stringData, separator);

  T tempValue;
  for (auto s : dataLine) {
    bool success = load_value(s, tempValue);
    if (!success) {
      throw std::invalid_argument("In convertVector() attempt to convert string to value failed\n");
    } else {
      returnData.push_back(tempValue);
    }
  }
}

/*
 * this is here so we can use to string and it will work even if we give it a string as input
 */
inline string to_string(string str) {
  return (str);
}

/*
 * getBestInVector(vector<T> vec)
 * given W (a list of scores), return the index of the highest score
 * if more then one genome has max score, return the first one.
 */
template<typename Type>
inline int findGreatestInVector(vector<Type> vec) {
  return distance(vec.begin(), max_element(vec.begin(), vec.end()));
}

#endif // __BasicMarkovBrainTemplate__Utilities__

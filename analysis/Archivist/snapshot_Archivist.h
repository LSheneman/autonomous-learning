//
//  snapshot_Archivist.h
//  BasicMarkovBrainTemplate
//
//  Created by Cliff Bohm on 5/30/15.
//

#ifndef __BasicMarkovBrainTemplate__snapshot_Archivist__
#define __BasicMarkovBrainTemplate__snapshot_Archivist__

#include "Archivist.h"

using namespace std;

class Snapshot_Archivist : public Default_Archivist {  // SnapShot
 public:
  static int& SS_Arch_dataInterval;  // how often to save data
  static int& SS_Arch_genomeInterval;  // how often to save genomes
  static bool& SS_Arch_writeDataFiles;  // if true, write data file
  static bool& SS_Arch_writeGenomeFiles;  // if true, write genome file

  int dataInterval;
  int genomeInterval;
  bool writeDataFiles;  // if true, write data file
  bool writeGenomeFiles;  // if true, write genome file

  Snapshot_Archivist()
      : Default_Archivist() {
    dataInterval = SS_Arch_dataInterval;
    genomeInterval = SS_Arch_genomeInterval;
    writeDataFiles = SS_Arch_writeDataFiles;
    writeGenomeFiles = SS_Arch_writeGenomeFiles;
  }

  ~Snapshot_Archivist() = default;

  void saveSnapshotData(vector<shared_ptr<Organism>> population, int update) {
    // write out data
    string dataFileName = "statsDataTest_" + to_string(update) + ".csv";

    if (files.find("data") == files.end()) {  // first make sure that the dataFile has been set up.
      files["data"] = population[0]->dataMap.getKeys();  // get all keys from the valid orgs dataMap (all orgs should have the same keys in their dataMaps)
    }
    for (auto org : population) {
      org->dataMap.writeToFile(dataFileName, files["data"]);  // append new data to the file
    }
  }

  void saveSnapshotGenomes(vector<shared_ptr<Organism>> population, int update) {

    // write out genomes
    string genomeFileName = "genomes_" + to_string(update) + ".csv";

    string dataString;
    for (auto org : population) {
      dataString = to_string(org->ID) + FileManager::separator + "\"[" + org->genome->convert_to_string() + "]\"";  // add interval update, genome ancestors, and genome with padding to string
      FileManager::writeToFile(genomeFileName, dataString, "ID,genome");  // write data to file
    }
  }

  virtual bool archive(vector<shared_ptr<Organism>> population, int flush = 0) {
      cout<<"We are in snapshot archivist"<<endl;
    if (flush != 1) {
      if (Global::update % realtimeFilesInterval == 0) {  // do not write files on flush - these organisms have not been evaluated!
        writeRealTimeFiles(population);  // write to dominant and average files
      }
      if ((Global::update % SS_Arch_dataInterval == 0) && (flush == 0) && writeDataFiles) {  // do not write files on flush - these organisms have not been evaluated!
        saveSnapshotData(population, Global::update);
      }
      if ((Global::update % SS_Arch_genomeInterval == 0) && (flush == 0) && writeGenomeFiles) {  // do not write files on flush - these organisms have not been evaluated!
        saveSnapshotGenomes(population, Global::update);
      }
      for (auto org : population) {  // we don't need to worry about tracking parents or lineage, so we clear out this data every generation.
        org->clearHistory();
      }
    }
    // if we are at the end of the run
    return (Global::update >= Global::updates);
  }

};

#endif /* defined(__BasicMarkovBrainTemplate__snapshot_Archivist__) */

//
//  LODwAP_Archivist.h
//  BasicMarkovBrainTemplate
//
//  Created by Cliff Bohm on 5/30/15.
//

#ifndef __BasicMarkovBrainTemplate__LODwAP_Archivist__
#define __BasicMarkovBrainTemplate__LODwAP_Archivist__

#include "Archivist.h"

using namespace std;

class LODwAP_Archivist : public Default_Archivist {  // Line of Decent with Active Pruning
 public:

  static int& LODwAP_Arch_dataInterval;  // how often to write out data
  static int& LODwAP_Arch_genomeInterval;  // how often to write out genomes
  static int& LODwAP_Arch_pruneInterval;  // how often to attempt to prune the LOD
  static int& LODwAP_Arch_terminateAfter;  // how long to run after updates (to get better coalescence)
  static string& LODwAP_Arch_DataFileName;  // name of the Data file
  static string& LODwAP_Arch_GenomeFileName;  // name of the Genome file (genomes on LOD)
  static bool& LODwAP_Arch_writeDataFile;  // if true, write data file
  static bool& LODwAP_Arch_writeGenomeFile;  // if true, write genome file

  int dataInterval;  // how often to write out data
  int genomeInterval;  // how often to write out genomes
  int pruneInterval;  // how often to attempt to prune the LOD
  int terminateAfter;  // how long to run after updates (to get better coalescence)
  string DataFileName;  // name of the Data file
  string GenomeFileName;  // name of the Genome file (genomes on LOD)
  bool writeDataFile;  // if true, write data file
  bool writeGenomeFile;  // if true, write genome file

  int lastPrune;  // last time Genome was Pruned

  //// info about files under management
  int nextDataWrite;  // next time data files will be written to disk
  int nextGenomeWrite;  // next time a genome file will be written to genome.csv
  int nextDataCheckPoint;  // next time data files contents need to be checkpointed (this is needed particularly to handle delay > interval)
  int nextGenomeCheckPoint;  // next time a genome files contents need to be checkpointed (this is needed particularly to handle delay > interval)

  //unordered_map<int, vector<weak_ptr<Organism>>> checkpoints;  // used by SSwD only - this keeps lists of orgs that may be written (if they have living decendents)
  //// key is Global::nextGenomeWrite or Global::nextDataWrite

  LODwAP_Archivist()
      : Default_Archivist() {

    dataInterval = LODwAP_Arch_dataInterval;
    genomeInterval = LODwAP_Arch_genomeInterval;
    pruneInterval = LODwAP_Arch_pruneInterval;
    terminateAfter = LODwAP_Arch_terminateAfter;
    DataFileName = LODwAP_Arch_DataFileName;
    GenomeFileName = LODwAP_Arch_GenomeFileName;
    writeDataFile = LODwAP_Arch_writeDataFile;
    writeGenomeFile = LODwAP_Arch_writeGenomeFile;

    nextDataWrite = 0;
    nextGenomeWrite = 0;
    nextDataCheckPoint = 0;
    nextGenomeCheckPoint = 0;

    lastPrune = 0;
  }

  virtual ~LODwAP_Archivist() = default;

  virtual bool archive(vector<shared_ptr<Organism>> population, int flush = 0) {

    if ((Global::update % dataInterval == 0) && (flush == 0)) {  // do not write files on flush - these organisms have not been evaluated!
      writeRealTimeFiles(population);  // write to dominant and average files
    }
    if ((Global::update % pruneInterval == 0) || (flush == 1)) {

      if (files.find("data.csv") == files.end()) {  // if file has not be initialized yet
        files[DataFileName] = population[0]->dataMap.getKeys();  // store keys from data map associated with file name
      }

      // get the MRCA
      vector<shared_ptr<Organism>> LOD = population[0]->getLOD(population[0]);  // get line of decent
      shared_ptr<Organism> effective_MRCA;
      if (flush) {  // if flush then we don't care about coalescence
        cout << "flushing LODwAP: using population[0] as MRCA\n";
        effective_MRCA = population[0]->parents[0];  // this assumes that a population was created, but not tested at the end of the evolution loop!
      } else {
        effective_MRCA = population[0]->getMostRecentCommonAncestor(LOD);  // find the convergance point in the LOD.
      }

      // Save Data
      if (writeDataFile) {
        while ((effective_MRCA->timeOfBirth >= nextDataWrite) && (nextDataWrite <= Global::updates)) {  // if there is convergence before the next data interval
          shared_ptr<Organism> current = LOD[nextDataWrite - lastPrune];
          for (auto file : files) {  // for each file in files
            current->dataMap.writeToFile(file.first, file.second);  // append new data to the file
          }
          nextDataWrite += dataInterval;
        }
      }

      //Save Genomes
      if (writeGenomeFile) {

        while ((effective_MRCA->timeOfBirth >= nextGenomeWrite) && (nextGenomeWrite <= Global::updates)) {  // if there is convergence before the next data interval
          shared_ptr<Organism> current = LOD[nextGenomeWrite - lastPrune];
          string dataString = to_string(nextGenomeWrite) + FileManager::separator + "\"[" + current->genome->convert_to_string() + "]\"";  // add write update and padding to genome string
          FileManager::writeToFile(GenomeFileName, dataString, "update,genome");  // write data to file
          nextGenomeWrite += genomeInterval;
        }
      }
      // data and genomes have now been written out up till the MRCA
      // so all data and genomes from before the MRCA can be deleted
      effective_MRCA->parents.clear();
      lastPrune = effective_MRCA->timeOfBirth;            // this will hold the time of the oldest genome in RAM
    }

    // if we have reached the end of time OR we have pruned past updates (i.e. written out all data up to updates), then we ae done!
    return (Global::update >= Global::updates + terminateAfter || lastPrune >= Global::updates);
  }

};

#endif /* defined(__BasicMarkovBrainTemplate__LODwAP_Archivist__) */

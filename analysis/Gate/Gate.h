//
//  Gate.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Gate__
#define __BasicMarkovBrainTemplate__Gate__

#include <deque>
#include <functional>
#include <math.h>
#include <memory>
#include <stdlib.h>
#include <vector>

#include "../Genome/Genome.h"
#include "../Global.h"

#include "../Utilities/Parameters.h"

using namespace std;

#define START_CODE 0
#define IN_COUNT_CODE 10
#define IN_ADDRESS_CODE 11
#define OUT_COUNT_CODE 20
#define OUT_ADDRESS_CODE 21
#define DATA_CODE 30

class Gate {  // abstact class. Assumes that a standard genome is being used.
 public:
    virtual double computeGateRMS(){return 0.0;}
    virtual double computeMutualInfo(){return 0.0;}
    virtual string getAppliedPosFeedback(){return "";}
    virtual string getAppliedNegFeedback(){return "";}
  static void AddGate(int ID, function<shared_ptr<Gate>(shared_ptr<Genome>, int)> theFunction);
  static void setupGates();
  static function<shared_ptr<Gate>(shared_ptr<Genome>, int)> makeGate[256];
    virtual string name(){return "not";}

  Gate()=default;
  virtual ~Gate()=default;

  vector<int> inputs;
  vector<int> outputs;

  map<int, int> codingRegions;  //indicates coding regions. maps genome index to usage of nucleotide in gate. 0=start codon. 1=wiring(defines # of inputs and outputs and what they are). 2=gate specific information

  // functions used in genome translation
  void movePastStartCodeon(int& genomeIndex, shared_ptr<Genome> genome);  // simply addvances genomeIndex by the size of a start codeon (2)
  int getIOAddress(int& genomeIndex, shared_ptr<Genome> genome);  // extracts one brain state value address from a genome
  void getSomeBrainAddresses(const int& howMany, const int& howManyMax, vector<int>& addresses, int& genome_index, shared_ptr<Genome> genome, int codeNumber);  // extracts many brain state value addresses from a genome
  void getInputsAndOutputs(const vector<int> insRange, vector<int> outsRange, int& genomeIndex, shared_ptr<Genome> genome);  // extracts the input and output brain state value addresses for this gate

  int getIndexFromInputs(vector<double> &brainState);  // used during update to convert gate input into table indexes

  /*
   * build a table of size range[0],range[1] which is the upper left subset of a table rangeMax[0],rangeMax[1]
   * the table rangeMax[0],rangeMax[0] would be filled with values from the genome (filling each row before going to the next column
   * This table is assigned to the gates table field.
   * set codingRegions for each used genome site value = DATA_CODE; (may need to add more support for this later!)
   */
  template<typename Type>
  void getTableFromGenome(vector<int> range, vector<int> rangeMax, int& genomeIndex, shared_ptr<Genome> genome, vector<vector<Type>> &table) {
    int x, y;
    int Y = range[0];
    int X = range[1];
    int maxY = rangeMax[0];
    int maxX = rangeMax[1];

    table.resize(Y);  // set the number of rows in the table

    for (y = 0; y < (Y); y++) {
      table[y].resize(X);  // set the number of columns in this row
      for (x = 0; x < X; x++) {
        table[y][x] = (Type) (genome->sites[genomeIndex]);
        codingRegions[genomeIndex] = DATA_CODE;
        genome->advanceIndex(genomeIndex);
      }
      for (; x < maxX; x++) {
        genome->advanceIndex(genomeIndex);  // advance genomeIndex to account for unused entries in the max sized table for this row
      }
    }
    genome->advanceIndex(genomeIndex, (maxY - Y) * maxX);  // advance genomeIndex to account for extra rows in the max sized table
  }

  virtual void applyNodeMap(vector<int> nodeMap, int maxNodes);  // converts genome values into brain state value addresses
  virtual void resetGate(void);  // this is empty here. Some gates so not need to reset, they can use this method.
  virtual vector<int> getIns();  // returns a vector of int with the adress for this gates input brain state value addresses
  virtual vector<int> getOuts();  // returns a vector of int with the adress for this gates onput brain state value addresses
  virtual string getCodingRegions();  // return a string with the codingRegions

  // functions which must be provided with each gate
  virtual void update(vector<double> & states, vector<double> & nextStates) = 0;  // the function is empty, and must be provided in any derived gates
  virtual string description() = 0;  // returns a description of the gate. This method can be used in derived gates description method to return ins and outs and coding regions

};

class ProbabilisticGate : public Gate {  //conventional probabilistic gate
 public:

  vector<vector<double>> table;

  ProbabilisticGate() = delete;
  ProbabilisticGate(shared_ptr<Genome> genome, int startCodonAt);

  virtual ~ProbabilisticGate() = default;

  virtual void update(vector<double> & states, vector<double> & nextStates);

  virtual string description();
    virtual string name();

};

class DeterministicGate : public Gate {
 public:
  vector<vector<int>> table;

  DeterministicGate() = delete;
  DeterministicGate(shared_ptr<Genome> genome, int startCodonAt);
  virtual ~DeterministicGate() = default;
  virtual void update(vector<double> & states, vector<double> & nextStates);

  void setupForBits(int* Ins, int nrOfIns, int Out, int logic);

  virtual string description();
    virtual string name();

  //double voidOutput;
};


class FixedEpsilonGate : public DeterministicGate {
 private:
 public:
  static double& FixedEpsilonGate_Probability;

  vector<int> defaultOutput;
  double epsilon;
  FixedEpsilonGate() = delete;
  FixedEpsilonGate(shared_ptr<Genome> genome, int startCodonAt);
  virtual ~FixedEpsilonGate() = default;
  virtual void update(vector<double> & states, vector<double> & nextStates);
  virtual string description();
};

class VoidGate : public DeterministicGate {
  static double& voidGate_Probability;

 private:
 public:
  vector<int> defaultOutput;
  double epsilon;
  VoidGate() = delete;
  VoidGate(shared_ptr<Genome> genome, int startCodonAt);
  virtual ~VoidGate() = default;
  virtual void update(vector<double> & states, vector<double> & nextStates);
  virtual string description();
    
};

#endif /* defined(__BasicMarkovBrainTemplate__Gate__) */

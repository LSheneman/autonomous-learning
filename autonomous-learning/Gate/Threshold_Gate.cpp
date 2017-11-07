//
//  Gate.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include <iostream>

#include "Gate.h"
#include "Threshold_Gate.h"

#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"

Thresholdgate::Thresholdgate(shared_ptr<Genome> genome, int startCodonAt) {
  int i, k;
  inputs.clear();
  outputs.clear();
  int _xDim;

  codingRegions.clear();

  codingRegions[startCodonAt] = START_CODE;
  codingRegions[(startCodonAt + 1) % genome->sites.size()] = START_CODE;
  //find the first nucleotide that isn't part of the start codon
  k = (startCodonAt + 2) % (int) genome->sites.size();

  //The dimensions are different here, the first inputs is the trigger
  //the next one are the current state Counter
  //the outputSize is the same, the first bit of the output is the ping
  //the others are the current state counter, they are the same as the inputs!
  //get the dimensions of the table
  codingRegions[k] = IN_COUNT_CODE;  //maps values of third and fourth nucleotide
  _xDim = 1 + (genome->sites[(k++) % genome->sites.size()] & 7);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  codingRegions[k] = OUT_COUNT_CODE;  ///////////////////////////////////////// IS THIS REALLY AN OUTPUT COUNT????
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //prepare the containers for the inputs and outputs addresses
  inputs.resize(_xDim);
  outputs.resize(_xDim);

  inputs[0] = genome->sites[k % genome->sites.size()];
  for (i = 1; i < _xDim; i++) {
    inputs[i] = genome->sites[(k + i) % genome->sites.size()];
    outputs[i] = genome->sites[(k + i) % genome->sites.size()];
  }
  //keep calm, i is at the right value
  outputs[0] = genome->sites[(k + i) % genome->sites.size()];

  //yes the threshold can be higher than the maximal number countable to by this threshold counter
  threshold = genome->sites[(k + i + 1) % genome->sites.size()];
}

void Thresholdgate::update(vector<double> & states, vector<double> & nextStates) {
  int theCount = 0;
  for (size_t i = 1; i < inputs.size(); i++)
    theCount = (theCount << 1) + Bit(states[inputs[i]]);
  theCount += Bit(states[inputs[0]]);
  if (theCount > threshold)
    states[outputs[0]] += 1.0;
  for (size_t i = 1; i < outputs.size(); i++)
    nextStates[outputs[i]] += 1.0 * ((theCount >> (i - 1)) & 1);
}

string Thresholdgate::description() {
  return "ThresholdgateGate: " + to_string(threshold) + "\n" + Gate::description();;
}


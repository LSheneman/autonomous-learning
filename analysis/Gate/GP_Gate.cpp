//
//  Gate.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include <iostream>

#include "GP_Gate.h"

#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"


/* *** GP Gate implementation *** */
union intToFloatBitByBit {
  int I;
  float F;
};

GPGate::GPGate(shared_ptr<Genome> genome, int genomeIndex) {
  int i;
  inputs.clear();
  outputs.clear();
  int numOutputs;
  codingRegions.clear();
  //move past the first to sites (start codeon)
  movePastStartCodeon(genomeIndex, genome);

  // get numInputs inputs and numOutputs outputs, advance k (genomeIndex) as if we had gotten 4 of each and record this in codingRegions
  getInputsAndOutputs( { 1, 4 }, { 1, 4 }, genomeIndex, genome);  // (insRange, outsRange,currentIndexInGenome,genome,codingRegions)
  numOutputs = outputs.size();

  genome->advanceIndex(genomeIndex, 16);

  codingRegions[genomeIndex] = DATA_CODE;
  myGateType = genome->sites[(genomeIndex++) % genome->sites.size()] % 8;
  myValues.clear();
  for (i = 0; i < numOutputs; i++) {
    intToFloatBitByBit V;
    V.I = 0;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    for (int j = 0; j < 4; j++) {  // what is this 4??? number of inputs?
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      codingRegions[genomeIndex] = DATA_CODE;
      V.I = (V.I << (int) 8) + (int) genome->sites[(genomeIndex++) % genome->sites.size()];
    }
    myValues.push_back(V.F);
  }
}

void GPGate::update(vector<double> & states, vector<double> & nextStates) {
  double retValue = inputs[0];
  int Z = 0;
  size_t i, o;
  bool convReturn = true;
  switch (myGateType) {
    case 0:  //constant
      convReturn = false;
      for (o = 0; o < outputs.size(); o++)
        nextStates[outputs[o]] = myValues[o];
      break;
    case 1:  // add
      for (i = 1; i < inputs.size(); i++)
        retValue += states[inputs[i]];
      break;
    case 2:  // sub
      for (i = 1; i < inputs.size(); i++)
        retValue -= states[inputs[i]];
      break;
    case 3:  // mult
      for (i = 1; i < inputs.size(); i++)
        retValue *= states[inputs[i]];
      break;
    case 4:  // div
      for (i = 1; i < inputs.size(); i++)
        retValue /= states[inputs[i]];
      break;
    case 5:  // sin
      convReturn = false;
      for (o = 0; o < outputs.size(); o++) {
        nextStates[outputs[o]] += sin(states[inputs[Z]]);
        Z = (Z + 1) % (int) inputs.size();
      }
      break;
    case 6:  // cos
      convReturn = false;
      for (o = 0; o < outputs.size(); o++) {
        nextStates[outputs[o]] += cos(states[inputs[Z]]);
        Z = (Z + 1) % (int) inputs.size();
      }
      break;
    case 7:  // log
      convReturn = false;
      for (o = 0; o < outputs.size(); o++) {
        if (inputs[Z] > 0.0)
          nextStates[outputs[o]] += log(states[inputs[Z]]);
        Z = (Z + 1) % (int) inputs.size();
      }
      break;
    case 8:  // exp
      convReturn = false;
      for (o = 0; o < outputs.size(); o++) {
        nextStates[outputs[o]] += exp(states[inputs[Z]]);
        Z = (Z + 1) % (int) inputs.size();
      }
      break;
  }
  if (convReturn) {
    for (size_t o = 0; o < outputs.size(); o++)
      nextStates[outputs[o]] += retValue;
  }
}

string GPGate::description() {
  string gateTypeName[8] = { "+", "-", "*", "/", "Sin", "Cos", "Log", "Exp" };
  return "Genetic Programming " + gateTypeName[myGateType] + "\n" + Gate::description();
}


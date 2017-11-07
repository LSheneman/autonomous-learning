//
//  Gate.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include <iostream>

#include "Gate.h"

#include "Feedback_Gate.h"

#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"

bool FeedbackGate::feedbackON = true;

FeedbackGate::FeedbackGate(shared_ptr<Genome> genome, int startCodonAt) {

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // need to move to new Get Address
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  int i, j, k;
  inputs.clear();
  outputs.clear();
  int _xDim, _yDim;

  //find the first nucleotide that isn't part of the start codon
  k = (startCodonAt + 2) % (int) genome->sites.size();

  //get the dimensions of the table
  _xDim = 1 + (genome->sites[(k++) % genome->sites.size()] & 3);
  _yDim = 1 + (genome->sites[(k++) % genome->sites.size()] & 3);

  //get the dimensions of the feedback buffer
  posFBNode = genome->sites[(k++) % genome->sites.size()];
  negFBNode = genome->sites[(k++) % genome->sites.size()];
  nrPos = genome->sites[(k++) % genome->sites.size()] & 3;
  nrNeg = genome->sites[(k++) % genome->sites.size()] & 3;

  //prepare the containers for the inputs and outputs addresses
  inputs.resize(_yDim);
  outputs.resize(_xDim);

  //prepare the containers for the feedback pipelines
  posLevelOfFB.resize(nrPos);
  negLevelOfFB.resize(nrNeg);

  //get the I/outputs addresses

  for (i = 0; i < _yDim; i++)
    inputs[i] = genome->sites[(k + i) % genome->sites.size()];
  for (i = 0; i < _xDim; i++)
    outputs[i] = genome->sites[(k + 4 + i) % genome->sites.size()];

  //get the Feedback forces
  for (i = 0; i < nrPos; i++)
    posLevelOfFB[i] = ((double) (1 + genome->sites[(k + 8 + i) % genome->sites.size()])) / 256.0;
  for (i = 0; i < nrNeg; i++)
    negLevelOfFB[i] = ((double) (1 + genome->sites[(k + 12 + i) % genome->sites.size()])) / 256.0;

  k = k + 16;
  //get all the values into the table
  table.resize(1 << _yDim);
  originalTable.resize(1 << _yDim);
  for (i = 0; i < (1 << _yDim); i++) {
    table[i].resize(1 << _xDim);
    originalTable[i].resize(1 << _xDim);
    double S = 0.0;
    for (j = 0; j < (1 << _xDim); j++) {
      table[i][j] = (double) genome->sites[(k + j + ((1 << _xDim) * i)) % genome->sites.size()];
      S += table[i][j];
    }
    //normalize the row
    if (S == 0.0) {
      for (j = 0; j < (1 << _xDim); j++)
        table[i][j] = 1.0 / (double) (1 << _xDim);
    } else {
      for (j = 0; j < (1 << _xDim); j++)
        table[i][j] /= S;
    }
    for (j = 0; j < (1 << _xDim); j++)
      originalTable[i][j] = table[i][j];
  }

  chosenInPos.clear();
  chosenInNeg.clear();
  chosenOutPos.clear();
  chosenOutNeg.clear();
}

void FeedbackGate::update(vector<double> & states, vector<double> & nextStates) {
  size_t i;
  double mod;

  //Apply the feedback
    //default feedback to cut off positive feedback comment section out
  if ((feedbackON) && (nrPos != 0) && (states[posFBNode] > 0.0)) {
      //random feedback
//if ((feedbackON) && (nrPos != 0) && (Random::P(0.5)){
      //constant feedback
      //if ((feedbackON) && (nrPos != 0)){
    for (i = 0; i < chosenInPos.size(); i++) {
      mod = Random::getDouble(1) * posLevelOfFB[i];
        appliedPosFB.push_back(mod);
      table[chosenInPos[i]][chosenOutPos[i]] += mod;
      double s = 0.0;
      for (size_t k = 0; k < table[chosenInPos[i]].size(); k++)
        s += table[chosenInPos[i]][k];
      for (size_t k = 0; k < table[chosenInPos[i]].size(); k++)
        table[chosenInPos[i]][k] /= s;
    }
  }
    //default feedback to cut off negative feedback comment section out
  if ((feedbackON) && (nrNeg != 0) && (states[negFBNode] > 0.0)) {
      //random feedback
      //if ((feedbackON) && (nrNeg!= 0) && (Random::P(0.5)){
      //constant feedback
      //if ((feedbackON) && (nrNeg != 0)){
    for (i = 0; i < chosenInNeg.size(); i++) {
      mod = Random::getDouble(1) * negLevelOfFB[i];
        appliedNegFB.push_back(mod);
      table[chosenInNeg[i]][chosenOutNeg[i]] -= mod;
      if (table[chosenInNeg[i]][chosenOutNeg[i]] < 0.001)
        table[chosenInNeg[i]][chosenOutNeg[i]] = 0.001;
      double s = 0.0;
      for (size_t k = 0; k < table[chosenInNeg[i]].size(); k++)
        s += table[chosenInNeg[i]][k];
      for (size_t k = 0; k < table[chosenInNeg[i]].size(); k++)
        table[chosenInNeg[i]][k] /= s;
    }
  }

  //do the logic of the gate
  int input = 0;
  int output = 0;
  double r = Random::getDouble(1);
  for (size_t i = 0; i < inputs.size(); i++)
    input = (input << 1) + Bit(states[inputs[i]]);
  while (r > table[input][output]) {
    r -= table[input][output];
    output++;
  }
  for (size_t i = 0; i < outputs.size(); i++)
    nextStates[outputs[i]] += 1.0 * ((output >> i) & 1);

  //remember the last actions for future feedback
  if (feedbackON) {
    chosenInPos.push_back(input);
    chosenInNeg.push_back(input);
    chosenOutPos.push_back(output);
    chosenOutNeg.push_back(output);
    while (chosenInPos.size() > nrPos) {
      chosenInPos.pop_front();
    }
    while (chosenOutPos.size() > nrPos) {
      chosenOutPos.pop_front();
    }
    while (chosenInNeg.size() > nrNeg) {
      chosenInNeg.pop_front();
    }
    while (chosenOutNeg.size() > nrNeg) {
      chosenOutNeg.pop_front();
    }
  }
}

string FeedbackGate::description() {
  string S = "pos node:" + to_string((int) posFBNode) + "\n neg node:" + to_string((int) negFBNode);
  return "Feedback Gate\n " + S + "\n" + Gate::description();
}

string FeedbackGate::name() {
    return "Feedback Gate";
}

void FeedbackGate::applyNodeMap(vector<int> nodeMap, int maxNodes) {
  Gate::applyNodeMap(nodeMap, maxNodes);
  posFBNode = nodeMap[posFBNode % maxNodes];
  negFBNode = nodeMap[negFBNode % maxNodes];
}

void FeedbackGate::resetGate() {
  chosenInPos.clear();
  chosenInNeg.clear();
  chosenOutPos.clear();
  chosenOutNeg.clear();
    appliedNegFB.clear();
    appliedPosFB.clear();
  for (size_t i = 0; i < table.size(); i++)
    for (size_t j = 0; j < table[i].size(); j++)
      table[i][j] = originalTable[i][j];
    string temp;
}

vector<int> FeedbackGate::getIns() {
  vector<int> R;
  R.insert(R.begin(), inputs.begin(), inputs.end());
  R.push_back(posFBNode);
  R.push_back(negFBNode);
  return R;
}

double FeedbackGate::computeGateRMS(){
    double sumDiff=0;
    double numPoints=0;
    for(int i=0;i<table.size();i++)
        for(int j=0;j<table[i].size();j++){
            sumDiff=sumDiff+pow((double)table[i][j]-(double)originalTable[i][j],2.0);
            numPoints=numPoints+1.0;
        }
    return sqrt(sumDiff*(1.0/(double)numPoints));
}

double FeedbackGate::computeMutualInfo(){
    vector<vector<double>> normalizedTable;
    vector<double>columns;
    vector<double> rows;
    double mutualInfo=0.0;
    
    normalizedTable.resize(table.size());
    rows.resize(table.size());
    columns.resize(table[0].size());
    for(int i=0;i<table.size();i++){
        rows[i]=0.0;
        normalizedTable[i].resize(table[i].size());
        for(int j=0;j<table[i].size();j++){
            if(j==0 &&i==0)
                columns[j]=0.0;
            normalizedTable[i][j]=(double)table[i][j]/table.size();
            columns[j]+=normalizedTable[i][j];
            rows[i]+=(normalizedTable[i][j]);
        }
    }
    
    for(int i=0;i<table.size();i++){
        for(int j=0;j<table[i].size();j++){
            if(normalizedTable[i][j]!=0){
                mutualInfo+=normalizedTable[i][j]*log2(normalizedTable[i][j]/(columns[j]*rows[i]));
            }
        }
    }
    return mutualInfo;
}

string FeedbackGate::getAppliedPosFeedback(){

    //save all positive feedback the gate has used
    string temp="";
    for(int i = 0; i<appliedPosFB.size(); i++)
        temp+=','+to_string(appliedPosFB[i]);
    appliedPosFB.clear();
    return temp;
}

string FeedbackGate::getAppliedNegFeedback(){
    
    //save all negative feedback the gate has used
    string temp="";
    for(int i = 0; i<appliedNegFB.size(); i++)
        temp+=','+to_string(appliedNegFB[i]);
    appliedNegFB.clear();
    return temp;
}
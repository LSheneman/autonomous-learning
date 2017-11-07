//
//  Agent.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "Brain.h"

#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"

#define SKIPGATE 0 // if 0, remove the skipgate code - FOR SPEED UP!

vector<int> Brain::defaultNodeMap;

int& Brain::defaultNrOfBrainStates = Parameters::register_parameter("brainSize", 15, "number of Brain Values", "BRAIN");

#if SKIPGATE==1 // if SKIPGATE is enabled, there is a chance a gate will not update
double& Brain::skipGate = Parameters::register_parameter("skipGate", 0.0, "chance that a gate will not fire", "BRAIN");
#endif // SKIPGATE

bool& Brain::serialProcessing = Parameters::register_parameter("serialProcessing", false, "sets brains to overwrite... right?", "BRAIN");
//////////////////////////////////////////
// required code to support abstract class
AbstractBrain::AbstractBrain() {
}
AbstractBrain::~AbstractBrain() {
}
void AbstractBrain::update() {
}
string AbstractBrain::description() {
  cout << "   In AbstractBrain::description() : is an abstract method and should not be used\n";
  string S = "";
  return (S);
}

vector<string> AbstractBrain::getStats() {
  cout << "   In AbstractBrain::getStats() : is an abstract method and should not be used\n";
  vector<string> dataPairs;
  return (dataPairs);
}
// required code to support abstract class
//////////////////////////////////////////

Brain::Brain(shared_ptr<Genome> genome, int _nrOfBrainStates) {  //this is a constructor. it is run whenever a new brain is created.
  nrOfBrainStates = _nrOfBrainStates;
  states.resize(nrOfBrainStates);
  nextStates.resize(nrOfBrainStates);
  gates.clear();
  for (size_t i = 0; i < genome->sites.size() - 1; i++) {
    const int S = genome->sites[i];
    if ((S + genome->sites[i + 1]) == 256) {
      if (Gate_Builder::makeGate[S] != nullptr) {
        gates.push_back(Gate_Builder::makeGate[S](genome, i));
      }
    }
  }
  inOutReMap();  // map ins and outs from genome values to brain states
}

shared_ptr<Brain> Brain::makeBrainFromGenome(shared_ptr<Genome> _genome) {
  shared_ptr<Brain> newBrain(new Brain(_genome, Brain::defaultNrOfBrainStates));
  return newBrain;
}

void Brain::resetBrain() {
  for (int i = 0; i < nrOfBrainStates; i++)
    states[i] = 0.0;
  for (size_t i = 0; i < gates.size(); i++)
    gates[i]->resetGate();
}

void Brain::update() {
  /*
   if(Agent::serialProcessing){
   //this is a special way of serialized updating
   //experimental feature, should be default off!
   //only change if you really know what you are doing!
   for(int i=0;i<gates.size();i++){
   gates[i]->update(states, states);
   }
   } else {*/
  //this is the default way of parallel updating
  nextStates.assign(nrOfBrainStates, 0.0);
//    for (int i = 0; i < nrOfBrainStates; i++) { //reset all states to 0
//        nextStates[i] = 0.0;
//    }
  for (size_t i = 0; i < gates.size(); i++) {  //update each gate
#if SKIPGATE==1 // if SKIPGATE is enabled, there is a chance a gate will not update
      if (!Random::P(Brain::skipGate)) {
        gates[i]->update(states, nextStates);
      }
#else
    gates[i]->update(states, nextStates);
#endif // SKIPGATE
  }
  swap(states, nextStates);
}

void Brain::inOutReMap() {  // remaps genome site values to valid brain state addresses
  for (size_t i = 0; i < gates.size(); i++) {
    gates[i]->applyNodeMap(defaultNodeMap, nrOfBrainStates);
  }

}

int Brain::IntFromState(vector<int> I) {
  int r = 0;
  for (size_t i = 0; i < I.size(); i++)
    r = (r << 1) + Bit(states[I[i]]);
  return r;
}

int Brain::IntFromAllStates() {
  int r = 0;
  for (int i = 0; i < nrOfBrainStates; i++)
    r = (r << 1) + Bit(states[i]);
  return r;

}

string Brain::description() {
  string S = "Markov Briain\n" + gateList();
  return S;
}

vector<string> Brain::getStats() {
  vector<string> dataPairs;
  dataPairs.push_back("gates");
  dataPairs.push_back(to_string(gates.size()));
    //
    int feedbackCount=0;
    for(auto gate:gates){
        if(gate->name()=="Feedback Gate"){
            feedbackCount+=1;
        }
    }
    dataPairs.push_back("feedbackCOunt");
    dataPairs.push_back(to_string(feedbackCount));
  return (dataPairs);
}

string Brain::gateList() {
  string S = "";
  for (size_t i = 0; i < gates.size(); i++) {
    S = S + gates[i]->description();
  }
  return S;
}

vector<vector<int>> Brain::getConnectivityMatrix() {
  vector<vector<int>> M;
  M.resize(nrOfBrainStates);
  for (int i = 0; i < nrOfBrainStates; i++) {
    M[i].resize(nrOfBrainStates);
    for (int j = 0; j < nrOfBrainStates; j++)
      M[i][j] = 0;
  }
  for (auto G : gates) {
    vector<int> I = G->getIns();
    vector<int> O = G->getOuts();
    for (int i : I)
      for (int o : O)
        M[i][o]++;
  }
  return M;
}

set<int> Brain::findCodingRegions(int mask) {  //if mask=0, all coding regions are returned. if mask=1, return everything except start codon. if mask=2, return everything except start codon and wiring.
  set<int> allCodingRegions;
  for (auto gateIterator = gates.begin(); gateIterator != gates.end(); gateIterator++) {
    for (auto iterator = (*gateIterator)->codingRegions.begin(); iterator != (*gateIterator)->codingRegions.end(); iterator++) {
      if (iterator->second >= mask) {
        allCodingRegions.insert(iterator->first);
      }
    }
  }
  return allCodingRegions;
}

int Brain::brainSize() {
  return (int) gates.size();
}

int Brain::numGates() {
  return brainSize();
}


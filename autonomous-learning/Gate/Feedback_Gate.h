//
//  Gate.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Feedback_Gate__
#define __BasicMarkovBrainTemplate__Feedback_Gate__

#include "Gate.h"

using namespace std;

class FeedbackGate : public Gate {
 private:
  vector<vector<double>> table;
  vector<vector<double>> originalTable;
  unsigned char posFBNode, negFBNode;
  unsigned char nrPos, nrNeg;
  vector<double> posLevelOfFB, negLevelOfFB;
  deque<unsigned char> chosenInPos, chosenInNeg, chosenOutPos, chosenOutNeg;
 public:

  static bool feedbackON;
  FeedbackGate() = delete;
  FeedbackGate(shared_ptr<Genome> genome, int startCodonAt);
  virtual ~FeedbackGate() = default;
  virtual void update(vector<double> & states, vector<double> & nextStates);
  virtual string description();
  virtual void applyNodeMap(vector<int> nodeMap, int maxNodes);
  virtual void resetGate(void);
  virtual vector<int> getIns();
    double computeGateRMS();
    string name();
};

#endif /* defined(__BasicMarkovBrainTemplate__Feedback_Gate__) */

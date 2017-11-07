//
//  Gate.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Threshold_Gate__
#define __BasicMarkovBrainTemplate__Threshold_Gate__
#include "Gate.h"

using namespace std;

class Thresholdgate : public Gate {
 private:
  int threshold;
 public:
  Thresholdgate() = delete;
  Thresholdgate(shared_ptr<Genome> genome, int startCodonAt);
  virtual ~Thresholdgate() = default;
  virtual void update(vector<double> & states, vector<double> & nextStates);
  virtual string description();
};

#endif /* defined(__BasicMarkovBrainTemplate__Threshold_Gate__) */

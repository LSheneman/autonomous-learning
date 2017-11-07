//
//  Gate.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__GP_Gate__
#define __BasicMarkovBrainTemplate__GP_Gate__

#include "Gate.h"

using namespace std;

class GPGate : public Gate {
 private:

  int myGateType;  //<link> stores the kind of GP operation (Add, Sub, Mult...)
  vector<double> myValues;								//<link>
 public:
  GPGate() = delete;
  GPGate(shared_ptr<Genome> genome, int startCodonAt);
  virtual ~GPGate() = default;
  virtual void update(vector<double> & states, vector<double> & nextStates);
  virtual string description();
};

#endif /* defined(__BasicMarkovBrainTemplate__GP_Gate__) */

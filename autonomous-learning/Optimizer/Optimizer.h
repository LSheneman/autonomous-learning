//
//  Optimizer.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Optimizer__
#define __BasicMarkovBrainTemplate__Optimizer__

#include <iostream>
#include <stdlib.h>
#include <vector>

#include "../Genome/Genome.h"
#include "../Organism/Organism.h"

#include "../Utilities/Parameters.h"

using namespace std;

class Optimizer {
 public:
  static int& elitism;
  static int& tournamentSize;

 public:
  double maxFitness;
  Optimizer() = default;
  virtual ~Optimizer() = default;
  virtual void makeNextGeneration(vector<shared_ptr<Organism>> &population);
};

class GA : public Optimizer {
 public:
  virtual void makeNextGeneration(vector<shared_ptr<Organism>> &population);
};

class P3 : public Optimizer {

};

class Tournament : public Optimizer {
 public:
  virtual void makeNextGeneration(vector<shared_ptr<Organism>> &population);
};

//
//****************************************************************************
//* code below this line is for testing - it will be deleted at some point.
//****************************************************************************
//
//

class Tournament2 : public Optimizer {
 public:
  virtual void makeNextGeneration(vector<shared_ptr<Organism>> &population);
};

#endif /* defined(__BasicMarkovBrainTemplate__Optimizer__) */

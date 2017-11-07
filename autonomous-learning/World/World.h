//
//  World.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__World__
#define __BasicMarkovBrainTemplate__World__

#include <stdlib.h>
#include <thread>
#include <vector>

#include "../Brain/Brain.h"
#include "../Organism/Organism.h"

using namespace std;

class World {
 public:
  static int& repeats;

 public:
  World();
  virtual ~World();
  virtual void evaluateFitness(vector<shared_ptr<Organism>> population, bool analyse);
  virtual double testIndividual(shared_ptr<Organism> org, bool analyse);
  virtual double testIndividual(shared_ptr<Organism> org, bool analyse, bool show){
    return testIndividual(org, analyse);
  }

};

//class ExampleWorld: public World {
//public:
//	ExampleWorld();
//	~ExampleWorld();
//	virtual void evaluateFitness(vector<Agent*> brains, bool analyse);
//	double testIndividual(Agent *brain, bool analyse);
//};
//
//class MultiThreadWorld: public World {
//private:
//	int nthreads;
//public:
////	void threadedEvaluateFitness(int chunkBegin, int chunkEnd, const vector<Agent*>& brains, int& evaluations);
//
//	MultiThreadWorld();
//	~ MultiThreadWorld();
//	virtual void evaluateFitness(vector<Organism*> population, bool analyse);
//	static void staticTestIndividual(Agent *brain, bool analyse);
//};
#endif /* defined(__BasicMarkovBrainTemplate__World__) */

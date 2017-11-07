//
//  World.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include <math.h>

#include "World.h"

#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"
#include "../Utilities/Data.h"

int& World::repeats = Parameters::register_parameter("repeats", 3, "Number of times to test each Genome per generation", "WORLD");

World::World() {
}

World::~World() {
}

void World::evaluateFitness(vector<shared_ptr<Organism>> population, bool analyse) {
  for (size_t i = 0; i < population.size(); i++) {
    double scoreTotal = 0.0;
    for (int r = 0; r < World::repeats; r++) {
      scoreTotal += testIndividual(population[i], analyse);
    }
    population[i]->score = (scoreTotal / (double) World::repeats);
    population[i]->dataMap.Set("score", population[i]->score);
    population[i]->dataMap.Set("update", Global::update);
  }
}

double World::testIndividual(shared_ptr<Organism> org, bool analyse) {
  return 1.0;
}

///* *** Example World Implementation *** */
//ExampleWorld::ExampleWorld() {
//}
//ExampleWorld::~ExampleWorld() {
//}
//
//vector<double> ExampleWorld::evaluateFitness(vector<Agent*> brains, bool analyse) {
//	vector<double> W;
//	for (size_t i = 0; i < brains.size(); i++) {
//		double w = 0.0;
//		for (int r = 0; r < World::repeats; r++) {
//			w += testIndividual(brains[i], analyse);
//		}
//		W.push_back(w / (double) World::repeats);
////		W.push_back(pow(w,1.0/(double)World::repeats)); // Arend, What does this mean?
//	}
//	return W;
//}
//
//double ExampleWorld::testIndividual(Agent *brain, bool analyse) {
//	double fitness = 0.0;
//	brain->resetBrain();
//	for (int t = 0; t < 100; t++) {
//		int I[8];
//		for (int i = 0; i < 3; i++) {
//			I[i] = Random::getInt(1);
//			brain->states[i] = (double) I[i];
//		}
//
//		brain->updateStates();
//		for (int i = 0; i < 3; i++) {
//			if (Bit(brain->states[8 + i]) == (int) I[i]) {
//				fitness += 1.0;
//			}
//		}
//	}
//	brain->genome->dataMap.Set("W", fitness);
//
//	return fitness;
//}
//
///* *** ADVANCED IMPLEMENTATION FOR MULTITHREADED RUNS *** */
///* *** CODE FOR INTEL PHI COPROCESSOR RUNS *** */
///* if you want to use this functionality you need to include pThreads
// and derive your world from this class! Don't overload evaluate Fitness,
// use the Replicates variable to run multiple replicates for each
// brain if necessary */
//
///*
// void threadedEvaluateFitness(int chunkBegin, int chunkEnd, const vector<Agent*>& brains, Game& game, int& evaluations){
// for (int chunk_index=chunkBegin; chunk_index<chunkEnd; chunk_index++) {
// for (int replicate_i=0; replicate_i < World::repeats; replicate_i++) {
// game.executeAgent(brains[chunk_index]);
// brains[chunk_index]->fitnesses.push_back(brains[chunk_index]->fitness);
// }
// }
// }
// */
//
//MultiThreadWorld::MultiThreadWorld() {
//	nthreads = thread::hardware_concurrency();
//	printf("Available Cores: %i\n", nthreads);
//}
//
//MultiThreadWorld::~MultiThreadWorld() {
//
//}
//
//void threadedEvaluateFitness(int chunkBegin, int chunkEnd, const vector<Agent*>& brains, int& evaluations) {
//	for (int chunk_index = chunkBegin; chunk_index < chunkEnd; chunk_index++) {
//		MultiThreadWorld::staticTestIndividual(brains[chunk_index], false);
//	}
//}
//
//vector<double> MultiThreadWorld::evaluateFitness(vector<Agent*> brains, bool analyse) {
//	vector<thread> threads;
//	vector<double> W;
//	threads.clear();
//	int chunksize = (int) brains.size() / nthreads;
//	for (int threadid = 0; threadid < nthreads; threadid++)
//		threads.push_back(
//				thread(threadedEvaluateFitness, chunksize * threadid, chunksize * threadid + chunksize, ref(brains),
//						ref(World::repeats)));
//
//	if (brains.size() % nthreads != 0) { // take care of any uneven division of workload
//		threads.push_back(
//				thread(threadedEvaluateFitness, nthreads * chunksize, brains.size(), ref(brains), ref(World::repeats)));
//	}
//	for (thread& t : threads)
//		t.join(); // wait for all threads to finish
//	for (size_t i = 0; i < brains.size(); i++) {
//		double thisW;
//		std::stringstream ss(brains[i]->genome->dataMap.Get("W"));
//		ss >> thisW; // use a string stream to convert string to double
//		W.push_back(thisW);
//	}
//	return W;
//}
//
//void MultiThreadWorld::staticTestIndividual(Agent *brain, bool analyse) {
//	brain->genome->dataMap.Set("W", Random::getDouble(1));
//}


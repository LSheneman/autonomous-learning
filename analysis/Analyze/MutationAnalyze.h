//
//  MutationAnalyse.h
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__MutationAnalyse__
#define __BasicMarkovBrainTemplate__MutationAnalyse__

#include <algorithm>
#include <iostream>
#include <fstream>
#include <set>
#include <time.h>
#include <stdlib.h>
#include <vector>

#include "../Brain/Brain.h"
#include "../Genome/Genome.h"
#include "../World/World.h"
#include "../World/BerryWorld.h"

#include "../Utilities/Data.h"

#include <sys/types.h>
#include <sys/stat.h>

using namespace std;
//class MutationAnalyse {
//public:
//	static vector<double> evalAgentNTimes(Agent* testAgent, World* world, int n);
//	static vector<set<int> > getPossibleStartCodons(Genome* genome);
//	static void evalPossibleAgents(Genome* genome, int brainStates, World *world, double &avgWildTypeFitness,
//			vector<double> &rawWildTypeFitness, map<int, vector<double>> &avePerVar, vector<double> &allVar,
//			vector<double> &avePerSite); //evaluates all possible variations for all genomes in a population
//
//	static void test_point_mutations_and_output(int gen, Genome* tester, bool avePerVar, bool allVar, bool wildTypeDist,
//			int nrOfBrainStates, World* world, string outputDirectory_Name);
//};

#endif /* defined(__BasicMarkovBrainTemplate__MutationAnalyse__) */

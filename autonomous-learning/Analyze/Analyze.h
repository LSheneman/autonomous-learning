//
//  Analyse.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 6/20/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Analyse__
#define __BasicMarkovBrainTemplate__Analyse__

#include <stdio.h>
#include <vector>
#include <map>
#include <math.h>
#include <set>

using namespace std;

class Analyse {
public:
	static double mutualInformation(vector<int> A, vector<int> B);
	static double ei(vector<int> A, vector<int> B, int theMask);
	static double computeAtomicPhi(vector<int> A, int states);
	static double computeR(vector<int> sensor, vector<int> brain, vector<int> environment, vector<int> total);
	static double entropy(vector<int> list);

	//connectivity matrix analysis
	static int getMaxConnectivityDensity(vector<vector<int>> M);
	static double getMeanConnectivityDensity(vector<vector<int>> M);
	static double getDensity(vector<vector<int>> M);
};

#endif /* defined(__BasicMarkovBrainTemplate__Analyse__) */

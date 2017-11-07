//
//  navWorld.h
//  MarkovBrain.4.0
//
//  Created by Leigh Sheneman on 1/26/16.
//  Copyright © 2016 Leigh Sheneman. All rights reserved.
//

#ifndef NavWorld_h
#define NavWorld_h

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

#include "../Brain/Brain.h"
#include "../Genome/Genome.h"
#include "../World/World.h"
#include "../Analyze/Analyze.h"
#include "../Utilities/Parameters.h"
#include "../Utilities/Utilities.h"
#include "../Utilities/Random.h"

#define xDim 64
#define yDim 64

#define empty 0
#define wall 1

const int xm4[4]={0,1,0,-1};
const int ym4[4]={-1,0,1,0};

struct Pos{
    int x,y;
    static Pos newPos(int X,int Y);
};


using namespace std;

class NavWorld: public World{
private:
    const int mapping[24][4]={
        {0,1,2,3} , {1,0,2,3} , {2,1,0,3} , {3,1,2,0},
        {0,1,3,2} , {1,0,3,2} , {2,1,3,0} , {3,1,0,2},
        {0,2,1,3} , {1,2,0,3} , {2,0,1,3} , {3,2,1,0},
        {0,2,3,1} , {1,2,3,0} , {2,0,3,1} , {3,2,0,1},
        {0,3,1,2} , {1,3,0,2} , {2,3,1,0} , {3,0,1,2},
        {0,3,2,1} , {1,3,2,0} , {2,3,0,1} , {3,0,2,1}
    };
    vector<vector<int> > area;
    vector<vector<int> > distMap;
    vector<vector<int> > dirMap;
    int targetX,targetY,startX,startY, currentMapID;
    int makeNumberNotBorder(int range);
    void makeMap();
public:
    NavWorld();
    static int& num_of_maps;
    void evaluateFitness(vector<shared_ptr<Organism>> population,bool analyse);
    double testIndividual(shared_ptr<Organism> org,bool analyse);
};
#endif /* NavWorld_h */

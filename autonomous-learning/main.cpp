//
//  main.cpp
//
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include <algorithm>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "Global.h"

#include "Archivist/Archivist.h"
#include "Archivist/LODwAP_Archivist.h"
#include "Archivist/snapshot_Archivist.h"
#include "Archivist/SSwD_Archivist.h"
#include "Brain/Brain.h"
#include "Genome/Genome.h"
#include "Group/Group.h"
#include "Optimizer/Optimizer.h"
#include "Organism/Organism.h"
#include "World/World.h"
#include "World/NavWorld.h"

#include "Utilities/Parameters.h"
#include "Utilities/Random.h"
#include "Utilities/Data.h"
#include "Utilities/Utilities.h"


using namespace std;

int main(int argc, const char * argv[]) {

  Parameters::initialize_parameters(argc, argv);  // loads command line and configFile values into registered parameters
                                                  // also writes out a config file if requested

  //make a node map to handle genome value to brain state address look up.
  Brain::makeNodeMap(Brain::defaultNodeMap, Global::bitsPerBrainAddress, Brain::defaultNrOfBrainStates);

  Gate_Builder::setupGates();  // determines which gate types will be in use.

  // outputDirectory must exist. If outputDirectory does not exist, no error will occur, but no data will be writen.
  FileManager::outputDirectory = Global::outputDirectory;


  if (Global::seedRandom) {
    random_device rd;
    Random::getCommonGenerator().seed(rd());
  } else {
    Random::getCommonGenerator().seed(Global::repNumber);
  }

  //Optimizer *optimizer = (Optimizer*) new GA();
  //Optimizer *optimizer = (Optimizer*) new Tournament();

  World *world = (World*) new NavWorld();  //new World();

////  ///// to show org in world
//  shared_ptr<Genome> testGenome = make_shared<Genome>();
//  testGenome->loadSites("genome.csv",1000);
//  shared_ptr<Organism> testOrg = make_shared<Organism>(testGenome, make_shared<Brain>());
//  world->testIndividual(testOrg,0,1);
//  exit(0);
////  ///// end to show org in world

  //////////////////
  // define population
  //////////////////

  shared_ptr<Group> group;

  {
    // a progenitor must exist - that is, one ancestor genome
    Global::update = -1;  // before there was time, there was a progenitor

    shared_ptr<Organism> progenitor = make_shared<Organism>(make_shared<Genome>(), make_shared<Brain>());  // make a organism with a genome and brain (if you need to change the types here is where you do it)

    Global::update = 0;  // the beginning of time - now we construct the first population
    vector<shared_ptr<Organism>> population;
    for (int i = 0; i < Global::popSize; i++) {
      shared_ptr<Genome> genome(new Genome());
      genome->fillRandom();
      shared_ptr<Organism> org(new Organism(progenitor, genome));
      population.push_back(org);  // add a new org to population using progenitors template and a new random genome
      population[population.size() - 1]->gender = Random::getInt(0, 1);  // assign a random gender to the new org
    }
    progenitor->kill();  // the progenitor has served it's purpose.

    shared_ptr<Default_Archivist> archivist;

    if (Default_Archivist::Arch_outputMethodStr == "default") {
      archivist = make_shared<Default_Archivist>();
    }
    if (Default_Archivist::Arch_outputMethodStr == "LODwAP") {
      archivist = make_shared<LODwAP_Archivist>();
    }
    if (Default_Archivist::Arch_outputMethodStr == "snapshot") {
      archivist = make_shared<Snapshot_Archivist>();
    }
    if (Default_Archivist::Arch_outputMethodStr == "SSwD") {
      archivist = make_shared<SSwD_Archivist>();
    }

    group = make_shared<Group>(population, make_shared<GA>(), archivist);
  }

//////////////////
// evolution loop
//////////////////

//  if (Archivist::outputMethod == -1) {  // this is the first time archive is called. get the output method
//    if (Archivist::outputMethodStr == "LODwAP") {
//      Archivist::outputMethod = 0;
//    } else if (Archivist::outputMethodStr == "SSwD") {
//      Archivist::outputMethod = 1;
//    } else {
//      cout << "unrecognized archive method \"" << Archivist::outputMethodStr << "\". Should be either \"LODwAP\" or \"SSwD\"\nExiting.\n";
//      exit(1);
//    }
//  }
  bool finished = false;  // when the archivist says we are done, we can stop!

  while (!finished) {
    world->evaluateFitness(group->population, false);  // evaluate each organism in the population using a World

    finished = group->archive();  // save data, update memory and delete any unneeded data;

    Global::update++;

    group->optimize();  // update the population (reproduction and death)

    cout << "update: " << Global::update - 1 << "   maxFitness: " << group->optimizer->maxFitness << "\n";
  }

  group->archive(1);  // flush any data that has not been output yet

//  if (Default_Archivist::Arch_outputMethodStr == "LODwAP") {  // if using LODwAP, write out some info about MRCA
//    shared_ptr<Organism> FinalMRCA = group->population[0]->getMostRecentCommonAncestor(group->population[0]);
//    cout << "MRCA - ID: " << FinalMRCA->ID << " born on: " << FinalMRCA->timeOfBirth << "\n" << FinalMRCA->brain->description();
//  }
  return 0;
}


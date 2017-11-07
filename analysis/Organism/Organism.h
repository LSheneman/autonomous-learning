//
//  Genome.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Organism__
#define __BasicMarkovBrainTemplate__Organism__

#include <stdlib.h>
#include <vector>
#include <unordered_set>

#include "../Brain/Brain.h"
#include "../Genome/Genome.h"

#include "../Utilities/Data.h"
#include "../Utilities/Parameters.h"

using namespace std;

class AbstractOrganism {
 public:
  AbstractOrganism();
  virtual ~AbstractOrganism() = 0;
};

class Organism : AbstractOrganism {
 private:
  static int organismIDCounter;  // used to issue unique ids to Genomes
  int registerOrganism();  // get an Organism_id (uses organismIDCounter)

 public:
  int gender;  // generally 0=female, 1=male, *=?
    double computeRMS();
    double computeMutualInfo();
    string getAppliedPosFeedback();
    string getAppliedNegFeedback();
  shared_ptr<Genome> genome;
  shared_ptr<Brain> brain;

  double score;

  int offspringCount;

  vector<shared_ptr<Organism>> parents;  // parents are pointers to parents of this organism. In asexual populations this will have one element
  unordered_set<int> genomeAncestors;  // list of the IDs of organisms in the last genome file who are ancestors of this organism (genomes saved on genome interval)
  unordered_set<int> dataAncestors;  // list of the IDs of organisms in the last data files who are ancestors of this organism (i.e. all files saved on data interval)

  int ID;
  int timeOfBirth;  // the time this organism was made
  int timeOfDeath;  // the time this organism stopped being alive (this organism may be stored for archival reasons)

  bool alive;  // is this organism alive (1) or dead (0)

  DataMap dataMap;  // holds all data (genome size, score, world data, etc.)
  unordered_map<int, DataMap> snapShotDataMaps;  // Used only with SnapShot with Delay (SSwD) stores contents of dataMap when an ouput interval is reached so that
  // after the delay we have the correct data for the given time. key is 'update'. This possibly should be wrapped into Archivist.
  Organism();  // make an empty organism
  Organism(shared_ptr<Genome> _genome);  // make a parentless organism with a genome, and a nullptr to brain
  Organism(shared_ptr<Genome> _genome, shared_ptr<Brain> _brain);  // make a parentless organism with a genome, and a brain
  Organism(shared_ptr<Organism> from, shared_ptr<Genome> _genome);  // make an organism with one parent, a genome and a brain determined from the parents brain type.
  Organism(const vector<shared_ptr<Organism>> from, shared_ptr<Genome> _genome);  // make a organism with many parents, a genome, and a brain determined from the parents brain type.

  virtual ~Organism();

  virtual void kill();  // sets alive = 0 (on org and in dataMap)

  virtual vector<string> GetLODItem(string key, shared_ptr<Organism> org);
  virtual vector<shared_ptr<Organism>> getLOD(shared_ptr<Organism> org);
  virtual shared_ptr<Organism> getMostRecentCommonAncestor(shared_ptr<Organism> org);
  virtual shared_ptr<Organism> getMostRecentCommonAncestor(vector<shared_ptr<Organism>> LOD);
  virtual shared_ptr<Organism> makeMutatedOffspring(shared_ptr<Organism> parent);
  virtual shared_ptr<Organism> makeMutatedOffspring(vector<shared_ptr<Organism>> from);
  virtual void clearHistory(); // clear all historical data (used when only saving real time data)
};

#endif /* defined(__BasicMarkovBrainTemplate__Organism__) */


//
//  Genome.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Genome__
#define __BasicMarkovBrainTemplate__Genome__

#include <stdlib.h>
#include <vector>

#include "../Utilities/Data.h"
#include "../Utilities/Parameters.h"

using namespace std;

class Genome : public std::enable_shared_from_this<Genome> {
 private:

 public:

  virtual vector<string> getStats();  // move me to abstract class!!

  /*
   * converts the sites vector to a FileManager::separator separated list in string format.
   */
  virtual string convert_to_string();  // move me to abstract class!!

  static int& initialGenomeSize;
  static double& pointMutationRate;
  static double& insertionRate;
  static double& deletionRate;
  static int& minGenomeSize;
  static int& maxGenomeSize;

  vector<unsigned char> sites;

  Genome() = default;
  Genome(vector<unsigned char> sites);
  Genome(shared_ptr<Genome> from);
  virtual ~Genome() = default;

  virtual void loadSites(string fileName, int update, int ID = -1);

  virtual void fillRandom();
  virtual void copyGenome(shared_ptr<Genome> from);
  virtual void applyMutations(double pointMutationRate = Genome::pointMutationRate, double insertionRate = Genome::insertionRate, double deletionRate = Genome::deletionRate,int minGenomeSize = Genome::minGenomeSize,int maxGenomeSize = Genome::maxGenomeSize);
  virtual shared_ptr<Genome> makeMutatedGenome();
  virtual shared_ptr<Genome> makeMutatedGenome(vector<shared_ptr<Genome>> from);

  virtual void makePointMutation();

  void advanceIndex(int& genomeIndex, int distance = 1) {
    genomeIndex = (genomeIndex + distance) % (int) sites.size();
  }

};

#endif /* defined(__BasicMarkovBrainTemplate__Genome__) */

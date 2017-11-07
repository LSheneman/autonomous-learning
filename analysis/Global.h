//
//  Global.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/31/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Global_
#define __BasicMarkovBrainTemplate__Global_

#include <map>
#include <memory>
#include <set>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

class Global {
 public:
  // parameters
  static int& repNumber;  // number used for output directory names and random seed
  static bool& seedRandom;  // if this is true, seed with a random value
  static int& updates;  // run until there is a MCRA at this time
  static int& popSize;  // number of genomes in the population

  static string& outputDirectory; // where files will be written

  static int& bitsPerBrainAddress;  // how many bits are evaluated to determine the brain addresses.
                                    // i.e. ceil(bitsPerBrainAddress/8) = number of genome sites used

  //////////
  // Below are non configurable values (not set directly by Parameters.h methods)
  //////////
  static int update;  // the current "time"
  static set<int> inUseGateTypes;  // this is global - a list of the start codeons for gates being used
};

#endif /* defined(__BasicMarkovBrainTemplate__Global__) */

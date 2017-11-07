//
//  Group.h
//  BasicMarkovBrainTemplate
//
//  Created by Cliff Bohm
//

#ifndef __BasicMarkovBrainTemplate__Group__
#define __BasicMarkovBrainTemplate__Group__

#include "../Archivist/Archivist.h"
#include "../Global.h"
#include "../Optimizer/Optimizer.h"
#include "../Organism/Organism.h"

class Group {
 public:
  vector<shared_ptr<Organism>> population;
  shared_ptr<Default_Archivist> archivist;
  shared_ptr<Optimizer> optimizer;

  Group();
  Group(vector<shared_ptr<Organism>> _population, shared_ptr<Optimizer> _optimizer, shared_ptr<Default_Archivist> _archivist);
  ~Group();

  bool archive(int flush = 0);
  void optimize();
};

#endif /* defined(__BasicMarkovBrainTemplate__Group__) */

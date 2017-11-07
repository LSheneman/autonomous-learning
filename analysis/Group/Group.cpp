/*
 * Group.cpp
 *
 *  Created on: Nov 18, 2015
 *      Author: cliff
 */

#include "../Group/Group.h"

Group::Group() {  // create an empty Group
}

Group::Group(vector<shared_ptr<Organism>> _population, shared_ptr<Optimizer> _optimizer,shared_ptr<Default_Archivist> _archivist) {
  population = _population;
  optimizer = _optimizer;
  archivist = _archivist;
}

Group::~Group() {
}

bool Group::archive(int flush) {
  return archivist->archive(population, flush);
}

void Group::optimize() {
  optimizer->makeNextGeneration(population);
}


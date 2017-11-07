//
//  BerryWorld.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 6/15/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "BerryWorld.h"

#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"

/* *** implementation of the World *** */

double& BerryWorld::TSK = Parameters::register_parameter("BERRY_taskSwitchingCost", 1.4, "cost to change food sources", "WORLD - BERRY");
int& BerryWorld::worldUpdates = Parameters::register_parameter("BERRY_WorldUpdates", 400, "amount of time an brain is tested", "WORLD - BERRY");

int& BerryWorld::foodTypes = Parameters::register_parameter("BERRY_foodSourceTypes", 2, "number of types of food", "WORLD - BERRY");
double& BerryWorld::rewardForFood1 = Parameters::register_parameter("BERRY_rewardForFood1", 1.0, "reward for eating a Food1", "WORLD - BERRY");
double& BerryWorld::rewardForFood2 = Parameters::register_parameter("BERRY_rewardForFood2", 1.0, "reward for eating a Food2", "WORLD - BERRY");
double& BerryWorld::rewardForFood3 = Parameters::register_parameter("BERRY_rewardForFood3", 1.0, "reward for eating a Food3", "WORLD - BERRY");
double& BerryWorld::rewardForFood4 = Parameters::register_parameter("BERRY_rewardForFood4", 1.0, "reward for eating a Food4", "WORLD - BERRY");
double& BerryWorld::rewardForFood5 = Parameters::register_parameter("BERRY_rewardForFood5", 1.0, "reward for eating a Food5", "WORLD - BERRY");
double& BerryWorld::rewardForFood6 = Parameters::register_parameter("BERRY_rewardForFood6", 1.0, "reward for eating a Food6", "WORLD - BERRY");
double& BerryWorld::rewardForFood7 = Parameters::register_parameter("BERRY_rewardForFood7", 1.0, "reward for eating a Food7", "WORLD - BERRY");
double& BerryWorld::rewardForFood8 = Parameters::register_parameter("BERRY_rewardForFood8", 1.0, "reward for eating a Food8", "WORLD - BERRY");

int& BerryWorld::WorldX = Parameters::register_parameter("BERRY_WorldX", 8, "world X size", "WORLD - BERRY");
int& BerryWorld::WorldY = Parameters::register_parameter("BERRY_WorldY", 8, "world Y size", "WORLD - BERRY");
bool& BerryWorld::borderWalls = Parameters::register_parameter("BERRY_makeBorderWalls", true, "if true world will have a bounding wall", "WORLD - BERRY");
int& BerryWorld::randomWalls = Parameters::register_parameter("BERRY_makeRandomWalls", 0, "add this many walls to the world", "WORLD - BERRY");

bool& BerryWorld::clearOutputs = Parameters::register_parameter("BERRY_clearOutputs", false, "if true outputs will be cleared on each world update", "WORLD - BERRY");

bool& BerryWorld::allowMoveAndEat = Parameters::register_parameter("BERRY_allowMoveAndEat", false, "if true, the brain can move and eat in the same world update", "WORLD - BERRY");
bool& BerryWorld::senseDown = Parameters::register_parameter("BERRY_senseDown", true, "if true, Agent can sense what it's standing on", "WORLD - BERRY");
bool& BerryWorld::senseFront = Parameters::register_parameter("BERRY_senseFront", true, "if true, Agent can sense what's in front of it", "WORLD - BERRY");

bool& BerryWorld::senseFrontSides = Parameters::register_parameter("BERRY_senseFrontSides", true, "if true, Agent can sense what's in front to the left and right of it", "WORLD - BERRY");
bool& BerryWorld::senseWalls = Parameters::register_parameter("BERRY_senseWalls", false, "if true, Agent can sense Walls", "WORLD - BERRY");

BerryWorld::BerryWorld() {
  senseWalls = senseWalls & (borderWalls | (randomWalls > 0));  // if there are no walls, there is no need to sense them!

  outputStatesCount = 3;  // number of brain states used for output, 2 for move, 1 for eat

  if (senseWalls) {
    inputStatesCount = (senseDown * foodTypes) + ((senseFront * foodTypes) + senseWalls) + (2 * ((senseFrontSides * foodTypes) + senseWalls));
    // sense down does not include walls (can't stand on a wall (yet!) * types of food
    // senseFront * types of food + wall, same for senseFrontSides, but there are 2
  } else {  // no border walls
    inputStatesCount = (senseDown * foodTypes) + (senseFront * foodTypes) + (2 * (senseFrontSides * foodTypes));
    // sense down * types of food, same for senseFront, same for senseFrontSides, but there are 2
  }

  cout << "  World using following BrainSates:\n    Inputs: 0 to " << inputStatesCount - 1 << "\n    Outputs: " << inputStatesCount << " to " << inputStatesCount + outputStatesCount - 1 << "\n";
}

void BerryWorld::printGrid(vector<int> grid, pair<int, int> loc, int facing) {
  for (int y = 0; y < BerryWorld::WorldY; y++) {
    for (int x = 0; x < BerryWorld::WorldX; x++) {
      if ((x == loc.first) && (y == loc.second)) {
        cout << facingDisplay[facing] << " ";
      } else {
        if (getGridValue(grid, { x, y }) == WALL) {
          cout << "X";
        } else {
          cout << getGridValue(grid, { x, y });
        }
        cout << " ";
      }
    }
    cout << "\n";
  }
  cout << "\n";
}

double BerryWorld::testIndividual(shared_ptr<Organism> org, bool analyse, bool show) {
  double score = 0.0;

  vector<int> grid = makeTestGrid();

  // organism starts in the center of the world, facing in a random direction.
  pair<int, int> currentLocation = { WorldX / 2, WorldY / 2 };  // location of the organism
  int facing = Random::getIndex(8);  // direction the agent is facing

  // set up to track what food is eaten
  int switches = 0;  // number of times organism has switched food source
  int lastFood = -1;  //nothing has been eaten yet!
  vector<int> eaten;  // stores number of each type of food was eaten in total for this test NOTE: food is indexed from 1 so 0th entry is not used
  eaten.resize(foodTypes + 1);

  vector<double> foodRewards;
  foodRewards.resize(9);  // stores reward of each type of food NOTE: food is indexed from 1 so 0th entry is not used
  foodRewards[1] = rewardForFood1;
  foodRewards[2] = rewardForFood2;
  foodRewards[3] = rewardForFood3;
  foodRewards[4] = rewardForFood4;
  foodRewards[5] = rewardForFood5;
  foodRewards[6] = rewardForFood6;
  foodRewards[7] = rewardForFood7;
  foodRewards[8] = rewardForFood8;

  org->dataMap.Clear("foodList");  // since foodList is built with Append, if an org lives for more then one "generation update" it's foodList must be cleared

  // set up vars needed to run
  int output1 = 0;  // store outputs from brain
  int output2 = 0;

  int here, leftFront, front, rightFront;  // store grid values relitive to organsism

  int statesAssignmentCounter;  // this world can has number of brainState inputs set by parameter. This counter is used while assigning inputs

  vector<int> stateCollector;  // used in the phi calculation

  // make sure the brain does not have values from last run
  org->brain->resetBrain();

  for (int t = 0; t < worldUpdates; t++) {  //run agent for "worldUpdates" brain updates

    here = getGridValue(grid, currentLocation);
    front = getGridValue(grid, moveOnGrid(currentLocation, facing));
    leftFront = getGridValue(grid, moveOnGrid(currentLocation, turnLeft(facing)));
    rightFront = getGridValue(grid, moveOnGrid(currentLocation, turnRight(facing)));

    statesAssignmentCounter = 0;  // get ready to start assigning inputs

    if (senseWalls) {
      if (senseDown) {
        for (int i = 0; i < foodTypes; i++) {  // fill first states with food values at here location
          org->brain->setState(statesAssignmentCounter++, (here == i + 1));
        }
      }
      if (senseFront) {
        for (int i = 0; i < foodTypes; i++) {  // fill first states with food values at front location
          org->brain->setState(statesAssignmentCounter++, (front == i + 1));
        }
        org->brain->setState(statesAssignmentCounter++, (front == WALL));
      }
      if (senseFrontSides) {
        for (int i = 0; i < foodTypes; i++) {  // fill first states with food values at front location
          org->brain->setState(statesAssignmentCounter++, (leftFront == i + 1));
          org->brain->setState(statesAssignmentCounter++, (rightFront == i + 1));
        }
        org->brain->setState(statesAssignmentCounter++, (leftFront == WALL));
        org->brain->setState(statesAssignmentCounter++, (rightFront == WALL));
      }
    } else {  // don't sense walls
      if (senseDown) {
        for (int i = 0; i < foodTypes; i++) {  // fill first states with food values at here location
          org->brain->setState(statesAssignmentCounter++, (here == i + 1));
        }
      }
      if (senseFront) {
        for (int i = 0; i < foodTypes; i++) {  // fill first states with food values at front location
          org->brain->setState(statesAssignmentCounter++, (front == i + 1));
        }
      }
      if (senseFrontSides) {
        for (int i = 0; i < foodTypes; i++) {  // fill first states with food values at front location
          org->brain->setState(statesAssignmentCounter++, (leftFront == i + 1));
          org->brain->setState(statesAssignmentCounter++, (rightFront == i + 1));
        }
      }
    }

    if (show) {
      cout << "\n----------------------------\n";
      cout << "\ngeneration update: " << Global::update << "  world update: " << t << "\n";
      cout << "inStates: ";
      for (int i = 0; i < inputStatesCount; i++) {
        cout << org->brain->getState(i);
      }
      cout << "\n\n  -- brain update --\n\n";
    }

    // inputStatesCount is not set to the first output Brain State Address. we will not move it until the next world update!
    if (clearOutputs) {
      org->brain->setState(inputStatesCount, 0.0);
      org->brain->setState(inputStatesCount + 1, 0.0);
      org->brain->setState(inputStatesCount + 2, 0.0);
    }

    if (analyse) {  // gather some data before and after running update
      int S = 0;
      for (int i = 0; i < inputStatesCount; i++)
        S = (S << 1) + Bit(org->brain->getState(i));
      org->brain->update();
      for (int i = inputStatesCount + outputStatesCount; i < org->brain->nrOfBrainStates; i++)
        S = (S << 1) + Bit(org->brain->getState(i));
      stateCollector.push_back(S);
    } else {
      org->brain->update();  // just run the update!
    }

    // set output values
    // output1 has info about the first 2 output bits these [00 eat, 10 left, 01 right, 11 move]
    output1 = Bit(org->brain->getState(inputStatesCount)) + (Bit(org->brain->getState(inputStatesCount + 1)) << 1);
    // output 2 has info about the 3rd output bit, which either does nothing, or is eat.
    output2 = Bit(org->brain->getState(inputStatesCount + 2));

    if ((output2 == 1) && (getGridValue(grid, currentLocation) != EMPTY)) {  // eat food here (if there is food here)
      if (lastFood != -1) {  // if some food has already been eaten
        if (lastFood != getGridValue(grid, currentLocation)) {  // if this food is different then the last food eaten
          score -= BerryWorld::TSK;  // pay the task switch cost
          switches++;
        }
      }
      lastFood = getGridValue(grid, currentLocation);  // remember the last food eaten
      score += foodRewards[getGridValue(grid, currentLocation)];  // you ate a food... good for you!
      eaten[getGridValue(grid, currentLocation)]++;  // track the number of each berry eaten
      org->dataMap.Append("foodList", getGridValue(grid, currentLocation));
      setGridValue(grid, currentLocation, 0);  // clear this location
    }
    if ((output2 == 0) || (allowMoveAndEat == 1)) {  // if we did not eat or we allow moving and eating in the same world update
      switch (output1) {
        case 0:  //nothing
          break;
        case 1:  //turn left
          facing = turnLeft(facing);
          break;
        case 2:  //turn right
          facing = turnRight(facing);
          break;
        case 3:  //move forward
          if (getGridValue(grid, moveOnGrid(currentLocation, facing)) != WALL) {  // if the proposed move is not a wall
            if (getGridValue(grid, currentLocation) == EMPTY) {  // if the current location is empty
              setGridValue(grid, currentLocation, Random::getInt(1, foodTypes));  // plant a red or blue food
            }
            currentLocation = moveOnGrid(currentLocation, facing);
          }
          break;
      }
    }

    if (show) {
      cout << "outStates: " << Bit(org->brain->getState(inputStatesCount)) << Bit(org->brain->getState(inputStatesCount + 1)) << Bit(org->brain->getState(inputStatesCount + 2)) << "\n";
      cout << "output1: " << output1 << "  output2: " << output2 << "\n";
      cout << "\n  -- world update --\n\n";
      printGrid(grid, currentLocation, facing);
      cout << "last: " << lastFood << " here: " << getGridValue(grid, currentLocation) << "\nloc: " << currentLocation.first << "," << currentLocation.second << "  facing: " << facing << "\n";
      cout << "score: " << score << "\nfood1: " << eaten[1] << "  food2: " << eaten[2] << "  switches: " << switches << "\n";
    }

  }  // end world evaluation loop

  if (score < 0.0) {
    score = 0.0;
  }

  if (!org->dataMap.fieldExists("foodList")) {  // if no food was eaten foodList will be empty.
    org->dataMap.Append("foodList", 0);  // put a 0 in it as a placeholder (0 is no food)
  }

  int total_eaten = 0;
  for (int i = 1; i <= foodTypes; i++) {
    total_eaten += eaten[i];
    string temp_name = "food" + to_string(i);  // make food names i.e. food1, food2, etc.
    org->dataMap.Set(temp_name, eaten[i]);
  }
  org->dataMap.Set("total", total_eaten);  // total food eaten (regardless of type)

  org->dataMap.Set("switches", switches);
  org->dataMap.Set("score", score);

  if (analyse) {
    org->dataMap.Set("phi", Analyse::computeAtomicPhi(stateCollector, org->brain->nrOfBrainStates));
  }

  return score;
}

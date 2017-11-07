//
//  Gate.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include <iostream>

#include "Gate.h"

#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"

double& FixedEpsilonGate::FixedEpsilonGate_Probability = Parameters::register_parameter("FixedEpsilonGate_Probability", 0.05, "chance that an output from a FixedEpsilonGate gate will be randomized", "GATES");
double& VoidGate::voidGate_Probability = Parameters::register_parameter("voidOutputGate_Probability", 0.05, "chance that an output from a void gate will be set to 0", "GATES");

/* *** General tools for All Gates *** */

void Gate::movePastStartCodeon(int& genomeIndex, shared_ptr<Genome> genome) {
  codingRegions[genomeIndex] = START_CODE;
  genome->advanceIndex(genomeIndex);
  codingRegions[genomeIndex] = START_CODE;
  genome->advanceIndex(genomeIndex);
}

/*
 * gets an address with Agent::bitsPerBrainAddress bits
 * reads the required number of sites and then trims the leading bits.
 * if bitsPerBrainAddress = 10 and the next two sites were 14 (00001110) and 25(00011001)
 * the result would be 0000111000011001 masked with 0000001111111111 = 0000001000011001 or 537
 */
int Gate::getIOAddress(int& genomeIndex, shared_ptr<Genome> genome) {
  int bitCount;
  int address = (int) genome->sites[genomeIndex];  // grab a byte from the genome
  genome->advanceIndex(genomeIndex);  // advance the genome_index

  for (bitCount = 8; Global::bitsPerBrainAddress > bitCount; bitCount += 8) {  // now, one site (8 bits) at a time, add sites to the address, until we have enough bits
    address = (address << 8) | (int) genome->sites[genomeIndex];  // shift the current value 8 bits left and add the next site
    genome->advanceIndex(genomeIndex);  // advance the genome_index
  }
  int bitMask = 0;
  for (bitCount = 0; bitCount < Global::bitsPerBrainAddress; bitCount++) {  // create the bit mask - this is used to trim off unwanted leading bits from the address
    bitMask = (bitMask << 1) | 1;
  }
  return address & bitMask;  // trim the address with the bitMask
}

/*
 * Gets "howMany" addresses, advances the genome_index buy "howManyMax" addresses and updates "codingRegions" with the addresses being used.
 */
void Gate::getSomeBrainAddresses(const int& howMany, const int& howManyMax, vector<int>& addresses, int& genome_index, shared_ptr<Genome> genome, int codeNumber) {
  int lastLocation = genome_index;  //make a note of where were are now so we can get codingRegions later
  for (auto i = 0; i < howMany; i++) {  // for the number of addresses we need
    addresses[i] = getIOAddress(genome_index, genome);  // get an address
  }
  while (lastLocation < genome_index) {  // fill in codingRegions between lastLocation and currLocation
    codingRegions[lastLocation] = codeNumber;  // with 10 (inputs) or 11 (outputs) (connections)
    lastLocation++;
  }
  genome_index += ((ceil(((double) Global::bitsPerBrainAddress) / 8.0)) * (howManyMax - howMany));
  // move the genome_index ahead to account not only the addresses we need, but the total number of possible addresses
}

/*
 * given a genome and a genomeIndex:
 * pull out the number a number of inputs, number of outputs and then that many inputs and outputs
 * if number of inputs or outputs is less then the max possible inputs or outputs skip the unused sites in the genome
 * there can not be more then 255 inputs or outputs without additional support (to read more sites to generate these numbers)
 */
void Gate::getInputsAndOutputs(const vector<int> insRange, vector<int> outsRange, int& genomeIndex, shared_ptr<Genome> genome) {  // (max#in, max#out,currentIndexInGenome,genome,codingRegions)

  codingRegions[genomeIndex] = IN_COUNT_CODE;
  int numInputs = insRange[0] + (genome->sites[genomeIndex] & (insRange[1] - 1));

  genome->advanceIndex(genomeIndex);
  codingRegions[genomeIndex] = OUT_COUNT_CODE;
  int numOutputs = outsRange[0] + (genome->sites[genomeIndex++] & (outsRange[1] - 1));
  genome->advanceIndex(genomeIndex);

  inputs.resize(numInputs);
  outputs.resize(numOutputs);
  if (insRange[1] > 0) {
    getSomeBrainAddresses(numInputs, insRange[1], inputs, genomeIndex, genome, 11);
  }
  if (outsRange[1] > 0) {
    getSomeBrainAddresses(numOutputs, outsRange[1], outputs, genomeIndex, genome, 21);
  }
}

/*
 * converts values attained from genome for inputs and outputs to vaild brain state ids
 * uses nodeMap to accomplish the remaping
 */
void Gate::applyNodeMap(vector<int> nodeMap, int maxNodes) {
  for (size_t i = 0; i < inputs.size(); i++) {
    inputs[i] = nodeMap[inputs[i]] % maxNodes;
  }
  for (size_t i = 0; i < outputs.size(); i++) {
    outputs[i] = nodeMap[outputs[i]] % maxNodes;
  }
}

/* useful function if you need to look up values from a table during update.
 * convert the inputs to an index by reading them one at a time and bitshifting the result
 * the first input is read last so that 1, 10 , 100 (etc.) all return 1. (01)
 * and 01, 010, 0100 (etc.) all return 2 (10)
 * and 11, 110, 1100 (etc.) all return 3 (11)
 * etc... hopefully you see the pattern
 * This, btw, will maintain output consistency for these lower values even if a gate gains or loses inputs.
 */
int Gate::getIndexFromInputs(vector<double> & brainState) {
  int index = 0;

  for (int i = (int) inputs.size() - 1; i >= 0; i--) {
    index = (index << 1) + Bit(brainState[inputs[i]]);
  }
  return index;
}

void Gate::resetGate(void) {
  //nothing to reset here!
}

vector<int> Gate::getIns() {
  return inputs;
}

vector<int> Gate::getOuts() {
  return outputs;
}

// display the getCodingRegions for a gate as (Index1,Type1,Index2,Type2...)
string Gate::getCodingRegions() {
  string S = "";
  for (auto site : codingRegions) {
    S = S + to_string(site.first) + ":" + to_string(site.second) + "  ";
  }
  S += "\n";
  return S;
}

string Gate::description() {
  string S = " Gate\n";
  S = S + "IN:";
  for (size_t i = 0; i < inputs.size(); i++)
    S = S + " " + to_string(inputs[i]);
  S = S + "\n";
  S = S + "OUT:";
  for (size_t i = 0; i < outputs.size(); i++)
    S = S + " " + to_string(outputs[i]);
  S = S + "\n";
  S = S + getCodingRegions();
  return S;
}


/* *** ProbilisticGate implementation *** */

ProbabilisticGate::ProbabilisticGate(shared_ptr<Genome> genome, int genomeIndex) {
  int i, j;
  inputs.clear();
  outputs.clear();
  int numOutputs, numInputs;
  codingRegions.clear();

  //move past the first to sites (start codeon)
  movePastStartCodeon(genomeIndex, genome);

  // get numInputs inputs and numOutputs outputs, advance k (genomeIndex) as if we had gotten 4 of each and record this in codingRegions
  getInputsAndOutputs( { 1, 4 }, { 1, 4 }, genomeIndex, genome);  // (insRange, outsRange,currentIndexInGenome,genome,codingRegions)
  numInputs = inputs.size();
  numOutputs = outputs.size();

  genome->advanceIndex(genomeIndex, 16);

  // get a table filled with values from the genome that has
  // rows = (the number of possible combinations of input values) and columns = (the number of possible combinations of output values)
  getTableFromGenome( { 1 << numInputs, 1 << numOutputs }, { 16, 16 }, genomeIndex, genome, table);

  //normalize each row
  for (i = 0; i < (1 << numInputs); i++) {  //for each row (each possible input bit string)
    // first sum the row
    double S = 0;
    for (j = 0; j < (1 << numOutputs); j++) {
      S += table[i][j];
    }
    // now normalize the row
    if (S == 0.0) {  //if all the inputs on this row are 0, then give them all a probability of 1/(2^(number of outputs))
      for (j = 0; j < (1 << numOutputs); j++)
        table[i][j] = 1.0 / (double) (1 << numOutputs);
    } else {  //otherwise divide all values in a row by the sum of the row
      for (j = 0; j < (1 << numOutputs); j++)
        table[i][j] /= S;
    }
  }

}

void ProbabilisticGate::update(vector<double> & states, vector<double> & nextStates) {  //this translates the input bits of the current states to the output bits of the next states
  int input = getIndexFromInputs(states);  // converts the input values into an index
  int outputColumn = 0;
  double r = Random::getDouble(1);  // r will determine with set of outputs will be chosen
  while (r > table[input][outputColumn]) {
    r -= table[input][outputColumn];  // this goes across the probability table in row for the given input and subtracts each
    // value in the table from r until r is less than a value it reaches
    outputColumn++;  // we have not found the correct output so move to the next output
  }
  for (size_t i = 0; i < outputs.size(); i++)  //for each output...
    nextStates[outputs[i]] += 1.0 * ((outputColumn >> (outputs.size() - 1 - i)) & 1);  // convert output (the column number) to bits and pack into next states
                                                                                       // but always put the last bit in the first input (to maintain consistancy)
}

string ProbabilisticGate::description() {
  string S = "ProbilisticGate" + Gate::description();
  return S;
}

string ProbabilisticGate::name() {
    return "Probilistic Gate";
}

/* *** Determistic Gate Implementation *** */

DeterministicGate::DeterministicGate(shared_ptr<Genome> genome, int genomeIndex) {
  inputs.clear();
  outputs.clear();
  int numOutputs, numInputs;
  codingRegions.clear();

  //move past the first to sites (start codeon)
  movePastStartCodeon(genomeIndex, genome);

  // get numInputs inputs and numOutputs outputs, advance k (genomeIndex) as if we had gotten 4 of each and record this in codingRegions
  getInputsAndOutputs( { 1, 4 }, { 1, 4 }, genomeIndex, genome);  // (insRange, outsRange,currentIndexInGenome,genome,codingRegions)
  numInputs = inputs.size();
  numOutputs = outputs.size();

  genome->advanceIndex(genomeIndex, 16);  //move forward 16 spaces in genome

  // get a table filled with values from the genome that has
  // numbr of rows = (the number of possible combinations of input values) and number of columns = (the number of output values)
  getTableFromGenome( { (1 << numInputs), numOutputs }, { 1 << 4, 4 }, genomeIndex, genome, table);

  // convert the table contents to bits
  for (int i = 0; i < (1 << numInputs); i++) {
    for (int o = 0; o < numOutputs; o++) {
      table[i][0] = table[i][0] & 1;  // convert even to 0 and odd to 1
      //if (table[i][0] == 1) cout << "* "; // uncomment for light show! //
      //else cout << "  ";                  // uncomment for light show! //

    }
  }
}

void DeterministicGate::setupForBits(int* Ins, int nrOfIns, int Out, int logic) {
  inputs.resize(nrOfIns);
  for (int i = 0; i < nrOfIns; i++)
    inputs[i] = Ins[i];
  outputs.resize(1);
  outputs[0] = Out;
  table.resize(1 << nrOfIns);
  for (int i = 0; i < (1 << nrOfIns); i++) {
    table[i].resize(1);
    table[i][0] = (logic >> i) & 1;
  }
}

void DeterministicGate::update(vector<double> & states, vector<double> & nextStates) {
  int input = getIndexFromInputs(states);  // converts the input values into an index
  for (size_t i = 0; i < outputs.size(); i++) {
    nextStates[outputs[i]] += table[input][i];
  }
}

string DeterministicGate::description() {
  return "Deterministic " + Gate::description();
}

string DeterministicGate::name() {
    return "Deterministic Gate";
}

/* *** Fixed Epison Gate *** */
/* this gate behaves like a deterministic gate with a constant externally set error which may cause the outputs to scramble */

FixedEpsilonGate::FixedEpsilonGate(shared_ptr<Genome> genome, int genomeIndex)
    : DeterministicGate(genome, genomeIndex) {  // use DeterministicGate constructor to build set up everything (including a table of 0s and 1s)
  epsilon = FixedEpsilonGate_Probability;  // in case you want to have different epsilon for different gates (who am I to judge?)

  // now to the specifics of this gate - we convert the table to a list of numbers (i.e. bitstrings) so we can do fast comparisons in the update
  defaultOutput.clear();  // clear the defaultOutput vector... the list of bitstrings
  for (size_t i = 0; i < table.size(); i++) {  //for every row of the table (which is 2^inputs)
    int D = 0;
    for (size_t j = 0; j < table[i].size(); j++) {  //for every space in the row
      D |= (Bit(table[i][j]) << j);  //translate the value in that space to a bit (0 or 1), then bitshift it [position in row] times, then take D, bit-or it with that number,
      //and make that the new D
    }
    defaultOutput.push_back(D);  //add D (which is a bitstring of length [2^outputs]) to defaultOutput. defaultoutputs is now of length (2^number of inputs)
  }
}

void FixedEpsilonGate::update(vector<double> & states, vector<double> & nextStates) {
  int input = getIndexFromInputs(states);  // converts the input values into an index
  if (Random::P(epsilon)) {  //if there will be an error
    //do a random output excluding the one otherwise given
    int output = 0;
    do {
      output = Random::getIndex(table.size());  // pick a bitstring (with the correct number of bits) i.e. one bit for each output
    } while (output == defaultOutput[input]);  // if you happen to pick the a bit string which happens to match the correct output, pick again
    for (size_t i = 0; i < outputs.size(); i++) {
      nextStates[outputs[i]] += 1.0 * (double) ((output >> i) & 1);
    }
  } else {
    //do the default output
    for (size_t i = 0; i < outputs.size(); i++) {
      nextStates[outputs[i]] += table[input][i];
    }
  }
}

string FixedEpsilonGate::description() {
  return "Fixed Epsilon " + to_string(epsilon) + "\n " + Gate::description();
}



/* *** VoidGate *** */
/* this gate behaves like a deterministic gate with a constant externally set error which may set a single output to 0 */

VoidGate::VoidGate(shared_ptr<Genome> genome, int genomeIndex)
    : DeterministicGate(genome, genomeIndex) {  // use DeterministicGate constructor to build set up everything (including a table of 0s and 1s)
  epsilon = voidGate_Probability;  // in case you want to have different epsilon for different gates (who am I to judge?)
}

void VoidGate::update(vector<double> & states, vector<double> & nextStates) {
  int input = getIndexFromInputs(states);  // converts the input values into an index
  vector<int> outputRow = table[input];

  if (Random::P(epsilon)) {
    outputRow[Random::getIndex(outputs.size())] = 0;  // pick one output randomly and set it to 0
  }
  for (size_t i = 0; i < outputs.size(); i++) {
    nextStates[outputs[i]] += outputRow[i];
  }
}

string VoidGate::description() {
  return "Void Gate " + to_string(epsilon) + "\n " + Gate::description();
}


#include "Global.h"
#include "Utilities/Parameters.h"

//global variables that should be accessible to all
int& Global::repNumber = Parameters::register_parameter("repNumber", 101, "Replicate ID and seed (if seedWithPID not set true)", "GLOBAL");
bool& Global::seedRandom = Parameters::register_parameter("seedRandom", false, "if seedRandom, random number generator will seeded randomly", "GLOBAL");
int& Global::updates = Parameters::register_parameter("updates", 200, "how long the program will run", "GLOBAL");
int& Global::popSize = Parameters::register_parameter("popSize", 100, "number of genomes in the populaiton", "GLOBAL");

string& Global::outputDirectory = Parameters::register_parameter("outputDirectory", (string)"./", "where files will be written", "GLOBAL");

int& Global::bitsPerBrainAddress = Parameters::register_parameter("bitsPerBrainAddress", 10, "how many bits are evaluated to determine the brain addresses", "BRAIN");

int Global::update = 0;
set<int> Global::inUseGateTypes;

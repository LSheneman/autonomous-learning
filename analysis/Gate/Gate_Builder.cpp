
#include "Gate_Builder.h"

bool& Gate_Builder::usingProbGate = Parameters::register_parameter("probGate", false, "set to true to enable probabilistic gates", "GATE TYPES");
bool& Gate_Builder::usingDetGate = Parameters::register_parameter("detGate", true, "set to true to enable deterministic gates?", "GATE TYPES");
bool& Gate_Builder::usingEpsiGate = Parameters::register_parameter("epsiGate", false, "set to true to enable epsilon gates", "GATE TYPES");
bool& Gate_Builder::usingVoidGate = Parameters::register_parameter("voidGate", false, "set to true to enable void gates", "GATE TYPES");


bool& Gate_Builder::usingFBGate = Parameters::register_parameter("fbGate", false, "set to true to enable feedback gates", "GATE TYPES");
bool& Gate_Builder::usingGPGate = Parameters::register_parameter("gpGate", false, "set to true to enable GP (what?) gates", "GATE TYPES");
bool& Gate_Builder::usingThGate = Parameters::register_parameter("thGate", false, "set to true to enable threshold gates", "GATE TYPES");

/*
 * setupGates() populates Gate::makeGate (a structure containing functions) with the constructors for various types of gates.
 * there are 256 possible gates identified each by a pair of codons (n followed by 256-n)
 * after initializing Gate::MakeGate, Gate::AddGate() adds values and the associated constructor function to Gate::MakeGate
 */
void Gate_Builder::setupGates() {
  for (int i = 0; i < 256; i++) {
    AddGate(i, nullptr);
  }
  if (usingProbGate) {
    AddGate(42, [](shared_ptr<Genome> genome,int pos) {return make_shared<ProbabilisticGate>(genome,pos);});
    Global::inUseGateTypes.insert(42);
  }
  if (usingDetGate) {
    AddGate(43, [](shared_ptr<Genome> genome,int pos) {return make_shared<DeterministicGate>(genome,pos);});
    Global::inUseGateTypes.insert(43);
  }
  if (usingFBGate) {
    AddGate(44, [](shared_ptr<Genome> genome,int pos) {return make_shared<FeedbackGate>(genome,pos);});
    Global::inUseGateTypes.insert(44);
  }
  if (usingGPGate) {
    AddGate(45, [](shared_ptr<Genome> genome,int pos) {return make_shared<GPGate>(genome,pos);});
    Global::inUseGateTypes.insert(45);
  }
  if (usingThGate) {
    AddGate(46, [](shared_ptr<Genome> genome,int pos) {return make_shared<Thresholdgate>(genome,pos);});
    Global::inUseGateTypes.insert(46);
  }
  if (usingEpsiGate) {
    AddGate(47, [](shared_ptr<Genome> genome,int pos) {return make_shared<FixedEpsilonGate>(genome,pos);});
    Global::inUseGateTypes.insert(47);
  }
  if (usingVoidGate) {
    AddGate(48, [](shared_ptr<Genome> genome,int pos) {return make_shared<VoidGate>(genome,pos);});
    Global::inUseGateTypes.insert(47);
  }

}

/* *** some c++ 11 magic to speed up translation from genome to gates *** */
function<shared_ptr<Gate>(shared_ptr<Genome>, int)> Gate_Builder::makeGate[256];

void Gate_Builder::AddGate(int ID, function<shared_ptr<Gate>(shared_ptr<Genome>, int)> theFunction) {
  makeGate[ID] = theFunction;
}
/* *** end - some c++ 11 magic to speed up translation from genome to gates *** */

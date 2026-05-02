#include <iostream>
#include "phase1.h"
#include "phase2.h"
#include "phase3.h"
#include "phase4.h"
#include "phase5.h"
#include "phase6.h"
#include "phase7.h"
#include "phase8.h"
#include "phase9.h"

using namespace std;

int main() {

    ConfigPhase cp;
    cp.loadConfig("config.txt");
    cp.cfg.frameCount = 4;   // force small frame count to show algo differences
    cp.printConfig();

    SimulatorPhase sim(cp.cfg);

    if (cp.cfg.replacementAlgo == "OPT")
        sim.preScan("trace.txt");

    sim.run("trace.txt");

    return 0;
}
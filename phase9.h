#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "phase1.h"
#include "phase2.h"
#include "phase3.h"
#include "phase4.h"
#include "phase5.h"
#include "phase6.h"
#include "phase7.h"
#include "phase8.h"

//simulator phase
using namespace std;
class SimulatorPhase {
public:
    SystemConfig   cfg;
    TLBPhase* tlb;
    PageTablePhase* pt;
    PageFaultPhase* pf;
    StatsPhase* stats;
    AlgoPhase* algo;
    string algorithm;   // FIFO, LRU, OPT
    vector<unsigned int> loadedPages;

    SimulatorPhase(SystemConfig c) {
        cfg = c;
        algorithm = c.replacementAlgo;
        tlb = new TLBPhase(cfg);
        pt = new PageTablePhase(cfg);
        pf = new PageFaultPhase(cfg, tlb, pt);
        stats = new StatsPhase(cfg);
        algo = new AlgoPhase(cfg);
    }

    ~SimulatorPhase() {
        delete tlb;
        delete pt;
        delete pf;
        delete stats;
        delete algo;
    }
    void preScan(string traceFile) {
        if (algorithm == "OPT")
            algo->opt.preScan(traceFile, cfg.offsetBits);
    }
    bool lastEvictWasDirty = false;
    unsigned int getFrame(unsigned int vpn) {
        lastEvictWasDirty = false;

        if (pf->hasFreeFrame()) {
            return pf->getFreeFrame();
        }
        unsigned int victimVPN;

        if (algorithm == "LRU") {
            victimVPN = algo->lru.evict();
        }
        else if (algorithm == "OPT") {
            victimVPN = algo->opt.evict(loadedPages);
        }
        else {
            victimVPN = pf->fifoQueue.front();
            pf->fifoQueue.pop();
        }
        if (pt->isDirty(victimVPN)) {
            lastEvictWasDirty = true;
            stats->diskWrites++;
            stats->totalTime += cfg.diskLatency;
        }

        unsigned int frame = pt->getFrame(victimVPN);
        pt->evictPage(victimVPN);
        tlb->invalidate(victimVPN);

        for (int i = 0; i < (int)loadedPages.size(); i++) {
            if (loadedPages[i] == victimVPN) {
                loadedPages.erase(loadedPages.begin() + i);
                break;
            }
        }

        return frame;
    }
    void processAccess(unsigned int address, char op) {
        unsigned int vpn = address >> cfg.offsetBits;
        unsigned int offset = address & cfg.offsetMask;
        //checking TLB
        int frame = tlb->lookup(vpn);
        if (frame != -1) {
            stats->recordTLBHit();
            // If write mark dirty
            if (op == 'W') pt->markDirty(vpn);
            if (algorithm == "LRU") algo->lru.access(vpn);
            if (algorithm == "OPT") algo->opt.advance();
            return;
        }
        if (pt->isValid(vpn)) {
            // PAGE HIT
            frame = pt->getFrame(vpn);
            tlb->insert(vpn, frame);
            stats->recordPageHit();

            if (op == 'W') pt->markDirty(vpn);

            if (algorithm == "LRU") algo->lru.access(vpn);
            if (algorithm == "OPT") algo->opt.advance();
            return;
        }

        // page fault
        unsigned int freeFrame = getFrame(vpn);
        //loading from disk
        pt->loadPage(vpn, freeFrame);
        tlb->insert(vpn, freeFrame);
        loadedPages.push_back(vpn);
        if (algorithm == "FIFO") pf->fifoQueue.push(vpn);
        if (algorithm == "LRU")  algo->lru.access(vpn);
        if (algorithm == "OPT")  algo->opt.advance();
        if (op == 'W') pt->markDirty(vpn);
        if (lastEvictWasDirty)
            stats->recordDirtyFault();
        else
            stats->recordPageFault();
    }
    void run(string traceFile) {
        ifstream file(traceFile);
        if (!file.is_open()) {
            cout << "ERROR: Cannot open trace file: " << traceFile << "\n";
            return;
        }
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            int spacePos = (int)line.find(' ');
            if (spacePos == -1) continue;
            string hexStr = line.substr(0, spacePos);
            string opStr = line.substr(spacePos + 1);

            // Validate op
            if (opStr != "R" && opStr != "W") continue;

            // Strip 0x
            if (hexStr.size() >= 2 && hexStr[1] == 'x')
                hexStr = hexStr.substr(2);

            // Validate hex
            bool valid = true;
            for (int i = 0; i < (int)hexStr.size(); i++) {
                char c = hexStr[i];
                if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')))
                {
                    valid = false; break;
                }
            }
            if (!valid) continue;

            unsigned int address = stoul(hexStr, nullptr, 16);

            processAccess(address, opStr[0]);
        }

        file.close();
        stats->printReport();
    }
};

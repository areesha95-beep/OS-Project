#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

using namespace std;
struct SystemConfig {
    unsigned long long physicalRamSize = 0;
    unsigned int       pageSize = 0;
    unsigned int       tlbSize = 0;
    unsigned long long tlbLatency = 1;
    unsigned long long ramLatency = 10;
    unsigned long long diskLatency = 10000000;
    string             replacementAlgo = "FIFO";
    // Derived
    unsigned int frameCount = 0;
    unsigned int offsetBits = 0;
    unsigned int offsetMask = 0;
};
class ConfigPhase {
public:

    SystemConfig cfg;
    void loadConfig(string filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "ERROR: Cannot open file: " << filename << "\n";
            return;
        }
        string line;
        while (getline(file, line)) {
            if (line.empty()) {
                continue;
            }
            int spacePos = line.find(' ');
            if (spacePos == -1) {
                continue;
            }
            string key = line.substr(0, spacePos);
            string value = line.substr(spacePos + 1);

            if (key == "physical_ram_size") cfg.physicalRamSize = stoull(value);
            else if (key == "page_size")         cfg.pageSize = stoul(value);
            else if (key == "tlb_size")          cfg.tlbSize = stoul(value);
            else if (key == "tlb_latency")       cfg.tlbLatency = stoull(value);
            else if (key == "ram_latency")       cfg.ramLatency = stoull(value);
            else if (key == "disk_latency")      cfg.diskLatency = stoull(value);
            else if (key == "replacement_algo")  cfg.replacementAlgo = value;
        }
        file.close();

        computeDerived();
    }

    void computeDerived() {
        cfg.frameCount = (unsigned int)(cfg.physicalRamSize / cfg.pageSize);
        cfg.offsetBits = (unsigned int)log2(cfg.pageSize);
        cfg.offsetMask = cfg.pageSize - 1;
    }
    void printConfig() {
        cout << "\n SYSTEM CONFIG \n";
        cout << "Physical RAM  : " << cfg.physicalRamSize / (1024 * 1024) << " MB\n";
        cout << "Page Size     : " << cfg.pageSize / 1024 << " KB\n";
        cout << "TLB Size      : " << cfg.tlbSize << " entries\n";
        cout << "TLB Latency   : " << cfg.tlbLatency << " ns\n";
        cout << "RAM Latency   : " << cfg.ramLatency << " ns\n";
        cout << "Disk Latency  : " << cfg.diskLatency << " ns\n";
        cout << "Algorithm     : " << cfg.replacementAlgo << "\n";
        cout << " Derived \n";
        cout << "Frame Count   : " << cfg.frameCount << "\n";
        cout << "Offset Bits   : " << cfg.offsetBits << "\n";
        cout << "Offset Mask   : 0x" << hex << cfg.offsetMask << dec << "\n";
    }
};

#pragma once
#include <iostream>
#include "phase1.h"

using namespace std;

//  AddressPhase, splits a virtual address into VPN + Offset
class AddressPhase {
public:

    SystemConfig cfg;   // copy of config to know offsetBits and offsetMask

    // Constructor, receives config from Phase 1
    AddressPhase(SystemConfig c) {
        cfg = c;
    }

    //1. Extract VPN from virtual address 
    unsigned int getVPN(unsigned int address) {
        return address >> cfg.offsetBits;
    }

    //,2. Extract Offset from virtual address
    unsigned int getOffset(unsigned int address) {
        return address & cfg.offsetMask;
    }

    //3. Check if address is within valid range
    bool isValid(unsigned int address) {
        unsigned int maxAddress = cfg.physicalRamSize - 1;
        if (address > maxAddress) {
            cout << "  ERROR!! Address 0x" << hex << address
                << " is OUT OF BOUNDS. Max allowed: 0x" << maxAddress << dec << "\n";
            return false;
        }
        return true;
    }

    //4. Print the split result 
    void printSplit(unsigned int address) {
        cout << "  Virtual Address : 0x" << hex << address << "\n";
        cout << "  VPN             : " << dec << getVPN(address)
            << "  (0x" << hex << getVPN(address) << ")\n";
        cout << "  Offset          : " << dec << getOffset(address)
            << "  (0x" << hex << getOffset(address) << ")\n" << dec;
    }
};

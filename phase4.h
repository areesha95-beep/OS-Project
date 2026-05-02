#pragma once
#include <iostream>
#include "phase1.h"

using namespace std;

//  TLBEntry, one row in the TLB cache
struct TLBEntry {
    bool         valid = false;
    unsigned int vpn = 0;
    unsigned int frameNumber = 0;
};


//  TLBPhase, small cache that stores recent VPN→Frame maps
class TLBPhase {
public:

    SystemConfig cfg;
    TLBEntry* entries;    // array of TLB entries
    int          size;       // total TLB slots
    int          nextSlot;   // for FIFO replacement inside TLB

    TLBPhase(SystemConfig c) {
        cfg = c;
        size = cfg.tlbSize;
        nextSlot = 0;
        entries = new TLBEntry[size];  // allocate TLB array
    }

    ~TLBPhase() {
        delete[] entries;
    }

    // 1. Search TLB for a VPN — returns frame if hit
    //       returns -1 if miss
    int lookup(unsigned int vpn) {
        for (int i = 0; i < size; i++) {
            if (entries[i].valid && entries[i].vpn == vpn) {
                return entries[i].frameNumber;   // TLB HIT
            }
        }
        return -1;   // TLB MISS
    }

    // 2. Insert a VPN, Frame mapping into TLB 
    // uses FIFO to replace oldest entry when full
    void insert(unsigned int vpn, unsigned int frame) {
        // First check if VPN already exists, update it
        for (int i = 0; i < size; i++) {
            if (entries[i].valid && entries[i].vpn == vpn) {
                entries[i].frameNumber = frame;
                return;
            }
        }
        // Insert at nextSlot (FIFO replacement)
        entries[nextSlot].valid = true;
        entries[nextSlot].vpn = vpn;
        entries[nextSlot].frameNumber = frame;
        nextSlot = (nextSlot + 1) % size;   // wrap around
    }

    //3. Invalidate a TLB entry (called when page evicted)
    void invalidate(unsigned int vpn) {
        for (int i = 0; i < size; i++) {
            if (entries[i].valid && entries[i].vpn == vpn) {
                entries[i].valid = false;
                entries[i].vpn = 0;
                entries[i].frameNumber = 0;
            }
        }
    }

    //4. Clear entire TLB 
    void flush() {
        for (int i = 0; i < size; i++) {
            entries[i].valid = false;
            entries[i].vpn = 0;
            entries[i].frameNumber = 0;
        }
        nextSlot = 0;
    }

    //5. Print TLB contents 
    void printTLB() {
        cout << "\n   TLB Contents \n";
        cout << "  Slot | Valid | VPN | Frame\n";
        cout << "  --------------------------\n";
        for (int i = 0; i < size; i++) {
            cout << "  " << i
                << "    |   " << entries[i].valid
                << "   |  " << entries[i].vpn
                << "   |  " << entries[i].frameNumber << "\n";
        }
        cout << "  ----------------------\n\n";
    }
};

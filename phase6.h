#pragma once
#include <iostream>
#include <queue>
#include "phase1.h"
#include "phase4.h"
#include "phase5.h"

using namespace std;
//page fault phase
class PageFaultPhase {
public:

    SystemConfig   cfg;
    TLBPhase* tlb;
    PageTablePhase* pt;

    queue<unsigned int> freeFrames;   // available physical frames
    queue<unsigned int> fifoQueue;    // tracks order pages were loaded (for FIFO)

    int diskReads = 0;
    int diskWrites = 0;
    int pageFaults = 0;

    PageFaultPhase(SystemConfig c, TLBPhase* t, PageTablePhase* p) {
        cfg = c;
        tlb = t;
        pt = p;

        // Fill free frame list with all frame indices 0 to frameCount-1
        for (unsigned int i = 0; i < cfg.frameCount; i++)
            freeFrames.push(i);
    }

    //1. Check if any free frame is available
    bool hasFreeFrame() {
        return !freeFrames.empty();
    }

    //2. Get a free frame from the list
    unsigned int getFreeFrame() {
        unsigned int frame = freeFrames.front();
        freeFrames.pop();
        return frame;
    }

    // 3. FIFO eviction — pick oldest loaded page as victim 
    unsigned int evictFIFO() {
        unsigned int victimVPN = fifoQueue.front();
        fifoQueue.pop();

        // Check dirty bit, if dirty write to disk first
        if (pt->isDirty(victimVPN)) {
            cout << "  [DISK WRITE] VPN " << victimVPN << " is dirty, writing back to disk\n";
            diskWrites++;
        }

        // Get the frame the victim was using
        unsigned int freedFrame = pt->getFrame(victimVPN);

        // Evict from page table and TLB
        pt->evictPage(victimVPN);
        tlb->invalidate(victimVPN);

        cout << "  [EVICT] VPN " << victimVPN << " evicted, frame " << freedFrame << " is now free\n";
        return freedFrame;
    }

    //4. Handle a page fault for a given VPN 
    unsigned int handleFault(unsigned int vpn) {
        pageFaults++;
        cout << "  [PAGE FAULT] VPN " << vpn << "\n";

        unsigned int frame;

        if (hasFreeFrame()) {
            // Free frame available, use it directly
            frame = getFreeFrame();
            cout << "  [FREE FRAME] Assigned frame " << frame << " to VPN " << vpn << "\n";
        }
        else {
            // No free frame, run FIFO eviction
            cout << "  [NO FREE FRAME] Running FIFO eviction...\n";
            frame = evictFIFO();
        }

        // Load page from disk into frame
        diskReads++;
        cout << "  [DISK READ] Loading VPN " << vpn << " from disk into frame " << frame << "\n";

        // Update page table
        pt->loadPage(vpn, frame);

        // Update TLB
        tlb->insert(vpn, frame);

        // Add to FIFO queue
        fifoQueue.push(vpn);

        return frame;
    }

    //5. Print free frame stats 
   void printStats() {
    cout << "\n  .... Page Fault Handler Stats ....\n";
    cout << "  Page Faults  : " << pageFaults << "\n";
    cout << "  Disk Reads   : " << diskReads << "\n";
    cout << "  Disk Writes  : " << diskWrites << "\n";
    cout << "  Free Frames  : " << freeFrames.size() << "\n";
    cout << "  ...................................\n\n";
}
};

#pragma once
#include <iostream>
#include "phase1.h"
using namespace std;

//this phase just track all te counters and calculate EAT
class StatsPhase {
public:

    SystemConfig cfg;
    //counters
    int totalAccesses = 0;
    int tlbHits       = 0;
    int tlbMisses     = 0;
    int pageFaults    = 0;
    int pageHits      = 0;
    int diskReads     = 0;
    int diskWrites    = 0;
    //simulated total time in nanoseconds
    unsigned long long totalTime = 0;
    StatsPhase(SystemConfig c) {
        cfg = c;
    }

    //record a TLB hit
    void recordTLBHit() {
        totalAccesses++;
        tlbHits++;
        //TLB hit cost = tlbLatency + ramLatency (fetch data from RAM)
        totalTime += cfg.tlbLatency + cfg.ramLatency;
    }
// TLB MIsss
    void recordPageHit() {
        totalAccesses++;
        tlbMisses++;
        pageHits++;
        // TLB miss cost = tlbLatency + ramLatency (page table) + ramLatency (data)
        totalTime += cfg.tlbLatency + cfg.ramLatency + cfg.ramLatency;
    }

    //recording a page fault
    void recordPageFault() {
        totalAccesses++;
        tlbMisses++;
        pageFaults++;
        diskReads++;
        // page fault cost = tlbLatency + ramLatency + diskLatency + ramLatency
        totalTime += cfg.tlbLatency + cfg.ramLatency + cfg.diskLatency + cfg.ramLatency;
    }

    //Record a page fault with dirty eviction 
    void recordDirtyFault() {
        totalAccesses++;
        tlbMisses++;
        pageFaults++;
        diskReads++;
        diskWrites++;
        // dirty fault = tlbLatency + ramLatency + diskLatency(write) + diskLatency(read) + ramLatency
        totalTime += cfg.tlbLatency + cfg.ramLatency + cfg.diskLatency + cfg.diskLatency + cfg.ramLatency;
    }
    double computeEAT() {
        if (totalAccesses == 0) return 0;
        return (double)totalTime / totalAccesses;
    }
    double tlbHitRate() {
        if (totalAccesses == 0) return 0;
        return (double)tlbHits / totalAccesses * 100.0;
    }

    double pageFaultRate() {
        if (totalAccesses == 0) return 0;
        return (double)pageFaults / totalAccesses * 100.0;
    }
    void printReport() {
    cout << "\n========================================\n";
    cout << "       PERFORMANCE REPORT               \n";
    cout << "========================================\n";
    cout << "  Total Accesses  : " << totalAccesses << "\n";
    cout << "  TLB Hits        : " << tlbHits << "\n";
    cout << "  TLB Misses      : " << tlbMisses << "\n";
    cout << "  Page Hits       : " << pageHits << "\n";
    cout << "  Page Faults     : " << pageFaults << "\n";
    cout << "  Disk Reads      : " << diskReads << "\n";
    cout << "  Disk Writes     : " << diskWrites << "\n";
    cout << "----------------------------------------\n";
    cout << "  TLB Hit Rate    : " << tlbHitRate() << " %\n";
    cout << "  Page Fault Rate : " << pageFaultRate() << " %\n";
    cout << "  Total Time      : " << totalTime << " ns\n";
    cout << "  EAT             : " << computeEAT() << " ns\n";
    cout << "========================================\n\n";
}
};

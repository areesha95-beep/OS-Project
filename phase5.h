#pragma once
#include <iostream>
#include <unordered_map>
#include "phase1.h"

using namespace std;

// ────────────────────────────────────────────────────────────
//  PageTableEntry — one row in the page table
// ────────────────────────────────────────────────────────────
struct PageTableEntry {
    bool         valid = false;  // is page in RAM?
    bool         dirty = false;  // was it written to?
    unsigned int frameNumber = 0;     // which physical frame
};


// ────────────────────────────────────────────────────────────
//  PageTablePhase — manages the page table
// ────────────────────────────────────────────────────────────
class PageTablePhase {
public:

    SystemConfig cfg;

    // key = VPN, value = PageTableEntry
    unordered_map<unsigned int, PageTableEntry> table;

    PageTablePhase(SystemConfig c) {
        cfg = c;
    }

    // ── 1. Check if VPN exists and is valid in RAM ───────────
    bool isValid(unsigned int vpn) {
        if (table.find(vpn) == table.end()) return false;
        return table[vpn].valid;
    }

    // ── 2. Get frame number for a VPN ────────────────────────
    unsigned int getFrame(unsigned int vpn) {
        return table[vpn].frameNumber;
    }

    // ── 3. Load a page into a frame (on page fault) ──────────
    void loadPage(unsigned int vpn, unsigned int frame) {
        table[vpn].valid = true;
        table[vpn].dirty = false;
        table[vpn].frameNumber = frame;
    }

    // ── 4. Mark a page as dirty (on write operation) ─────────
    void markDirty(unsigned int vpn) {
        if (table.find(vpn) != table.end())
            table[vpn].dirty = true;
    }

    // ── 5. Check if a page is dirty ──────────────────────────
    bool isDirty(unsigned int vpn) {
        if (table.find(vpn) == table.end()) return false;
        return table[vpn].dirty;
    }

    // ── 6. Evict a page (invalidate it) ──────────────────────
    void evictPage(unsigned int vpn) {
        if (table.find(vpn) != table.end()) {
            table[vpn].valid = false;
            table[vpn].dirty = false;
            table[vpn].frameNumber = 0;
        }
    }

    // ── 7. Print a single page table entry ───────────────────
    void printEntry(unsigned int vpn) {
        if (table.find(vpn) == table.end()) {
            cout << "  VPN " << vpn << " : NOT IN TABLE\n";
            return;
        }
        PageTableEntry& e = table[vpn];
        cout << "  VPN " << vpn
            << "  |  Valid=" << e.valid
            << "  Dirty=" << e.dirty
            << "  Frame=" << e.frameNumber << "\n";
    }

    // ── 8. Print entire page table ───────────────────────────
    void printTable() {
        cout << "\n  ---- Page Table ----\n";
        cout << "  VPN  | Valid | Dirty | Frame\n";
        cout << "  -------------------------\n";
        for (auto& row : table) {
            cout << "  " << row.first
                << "     |   " << row.second.valid
                << "   |   " << row.second.dirty
                << "   |   " << row.second.frameNumber << "\n";
        }
        cout << "  --------------------\n\n";
    }
};
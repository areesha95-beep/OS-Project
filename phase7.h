#pragma once
#include <iostream>
#include <unordered_map>
#include <list>
#include <vector>
#include <fstream>
#include <string>
#include "phase1.h"
using namespace std;
class LRUReplacer {
public:

    int capacity;
    list<unsigned int>                                        lruList;
    unordered_map<unsigned int, list<unsigned int>::iterator> lruMap;
    LRUReplacer(int cap) {
        capacity = cap;
    }

    // Called every time a page is accessed
    void access(unsigned int vpn) {
        // if already in list remove it first
        if (lruMap.find(vpn) != lruMap.end())
            lruList.erase(lruMap[vpn]);
        // push to front (most recently used)
        lruList.push_front(vpn);
        lruMap[vpn] = lruList.begin();
    }
    // Evict least recently used (back of list)
    unsigned int evict() {
        unsigned int victim = lruList.back();
        lruList.pop_back();
        lruMap.erase(victim);
        return victim;
    }
    // Remove a specific VPN manually
    void remove(unsigned int vpn) {
        if (lruMap.find(vpn) != lruMap.end()) {
            lruList.erase(lruMap[vpn]);
            lruMap.erase(vpn);
        }
    }
    bool isFull() {
        return (int)lruList.size() >= capacity;
    }
    void printOrder() {
        cout << "  LRU Order (MRU = LRU): ";
        for (unsigned int vpn : lruList)
            cout << vpn << " ";
        cout << "\n";
    }
};
class OPTReplacer {
public:

    unordered_map<unsigned int, vector<int>> future;  // vpn, list of future positions
    int currentIndex = 0;

    // Step 1: read entire trace and record every access position per VPN
    void preScan(string filename, int offsetBits) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "  [OPT ERROR] Cannot open: " << filename << "\n";
            return;
        }

        string line;
        int pos = 0;

        while (getline(file, line)) {
            if (line.empty()) continue;

            int spacePos = line.find(' ');
            if (spacePos == -1) continue;

            string hexStr = line.substr(0, spacePos);
            // strip 0x prefix

            if (hexStr.size() >= 2 && hexStr[1] == 'x')
                hexStr = hexStr.substr(2);

            // validate hex chars
            bool valid = true;
            for (int i = 0; i < (int)hexStr.size(); i++) {
                char c = hexStr[i];
                if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')))
                {
                    valid = false; break;
                }
            }
            if (!valid) { pos++; continue; }

            unsigned int address = stoul(hexStr, nullptr, 16);
            unsigned int vpn = address >> offsetBits;
            future[vpn].push_back(pos);
            pos++;
        }
        file.close();
        cout << "  [OPT] Pre-scan done. Unique VPNs: " << future.size() << "\n";
    }

    // Step 2: given the currently loaded pages pick victim
    unsigned int evict(vector<unsigned int>& loadedPages) {
        int farthest = -1;
        unsigned int victim = loadedPages[0];

        for (unsigned int vpn : loadedPages) {
            int nextUse = getNextUse(vpn);

            // never used again = perfect victim
            if (nextUse == -1) return vpn;

            if (nextUse > farthest) {
                farthest = nextUse;
                victim = vpn;
            }
        }
        return victim;
    }
    void advance() { currentIndex++; }
    int getNextUse(unsigned int vpn) {
        if (future.find(vpn) == future.end()) return -1;
        vector<int>& uses = future[vpn];
        for (int i = 0; i < (int)uses.size(); i++)
            if (uses[i] > currentIndex) return uses[i];
        return -1;
    }
};
class AlgoPhase {
public:
    SystemConfig cfg;
    LRUReplacer  lru;
    OPTReplacer  opt;
    AlgoPhase(SystemConfig c) : cfg(c), lru(c.frameCount) {}
};

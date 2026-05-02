#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "phase1.h"

using namespace std;

// ────────────────────────────────────────────────────────────
//  TraceEntry — one line from the trace file
// ────────────────────────────────────────────────────────────
struct TraceEntry {
    unsigned int address = 0;
    char         op = ' ';   // 'R' or 'W'
    bool         valid = false;
};


// ────────────────────────────────────────────────────────────
//  TracePhase — reads and validates the trace file
// ────────────────────────────────────────────────────────────
class TracePhase {
public:

    SystemConfig cfg;

    int totalLines = 0;
    int validLines = 0;
    int invalidLines = 0;

    TracePhase(SystemConfig c) {
        cfg = c;
    }

    // ── 1. Convert hex string to unsigned int ────────────────
    //  "0x1ABC" or "1ABC"  →  6844
    bool hexToInt(string hexStr, unsigned int& result) {

        // Remove "0x" or "0X" prefix if present
        if (hexStr.size() >= 2 && hexStr[0] == '0' && (hexStr[1] == 'x' || hexStr[1] == 'X'))
            hexStr = hexStr.substr(2);

        // Check every character is a valid hex digit
        for (int i = 0; i < hexStr.size(); i++) {
            char c = hexStr[i];
            bool isDigit = (c >= '0' && c <= '9');
            bool isLower = (c >= 'a' && c <= 'f');
            bool isUpper = (c >= 'A' && c <= 'F');
            if (!isDigit && !isLower && !isUpper) {
                return false;   // invalid character found
            }
        }

        // Convert using stoul with base 16
        result = stoul(hexStr, nullptr, 16);
        return true;
    }

    // ── 2. Check if address is within physical RAM ───────────
    bool isInBounds(unsigned int address) {
        return address < cfg.physicalRamSize;
    }

    // ── 3. Check if operation is R or W ─────────────────────
    bool isValidOp(string op) {
        return (op == "R" || op == "W");
    }

    // ── 4. Parse one line into a TraceEntry ──────────────────
    TraceEntry parseLine(string line) {
        TraceEntry entry;

        // Find space between address and operation
        int spacePos = line.find(' ');
        if (spacePos == -1) {
            cout << "  [ERROR] Bad format: " << line << "\n";
            return entry;
        }

        string hexPart = line.substr(0, spacePos);
        string opPart = line.substr(spacePos + 1);

        // Validate hex address
        unsigned int address = 0;
        if (!hexToInt(hexPart, address)) {
            cout << "  [ERROR] Invalid hex address: " << hexPart << "\n";
            return entry;
        }

        // Validate operation
        if (!isValidOp(opPart)) {
            cout << "  [ERROR] Invalid operation '" << opPart << "' at address " << hexPart << "\n";
            return entry;
        }

        // Validate bounds
        if (!isInBounds(address)) {
            cout << "  [ERROR] Out of bounds: " << hexPart << "\n";
            return entry;
        }

        // All checks passed
        entry.address = address;
        entry.op = opPart[0];
        entry.valid = true;
        return entry;
    }

    // ── 5. Read and validate entire trace file ───────────────
    void validateFile(string filename) {
        ifstream file(filename);

        if (!file.is_open()) {
            cout << "ERROR: Cannot open trace file: " << filename << "\n";
            return;
        }

        cout << "  Validating trace file: " << filename << "\n\n";

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            totalLines++;

            TraceEntry entry = parseLine(line);
            if (entry.valid)
                validLines++;
            else
                invalidLines++;
        }

        file.close();
        printStats();
    }

    // ── 6. Print validation summary ──────────────────────────
    void printStats() {
        cout << "\n  ---- Trace Validation Summary ----\n";
        cout << "  Total Lines   : " << totalLines << "\n";
        cout << "  Valid Lines   : " << validLines << "\n";
        cout << "  Invalid Lines : " << invalidLines << "\n";
        cout << "  ----------------------------------\n";
    }
};
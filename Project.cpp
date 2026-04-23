
//  EC-210 FINAL PROJECT — FULL INTEGRATED SOLUTION
//  Virtual Memory + TLB Simulator 

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <queue>
#include <vector>
#include <list>
#include <iomanip>
#include <cmath>

using namespace std;

// CONFIG

struct Config {
    int pageSize;
    int ramSize;
    int tlbSize;
    int numFrames;
    int offsetBits;
};

class ConfigParser {
public:
    static Config load(string file) {
        Config c;
        ifstream f(file);
        string line;

        while (getline(f, line)) {
            string key, val;
            stringstream ss(line);
            getline(ss, key, '=');
            getline(ss, val);

            if (key == "page_size") c.pageSize = stoi(val);
            if (key == "ram_size")  c.ramSize  = stoi(val);
            if (key == "tlb_size")  c.tlbSize  = stoi(val);
        }

        c.numFrames = c.ramSize / c.pageSize;
        c.offsetBits = log2(c.pageSize);
        return c;
    }
};

// ADDRESS SPLITTER

class AddressSplitter {
public:
    static int hexToInt(string s) {
        return stoi(s, nullptr, 16);
    }
    static int getVPN(int addr, int shift) {
        return addr >> shift;
    }
};

// TRACE READER

struct Access {
    int vpn;
    bool write;
};

vector<Access> loadTrace(string file, int shift) {
    vector<Access> t;
    ifstream f(file);
    string op, addr;

    while (f >> op >> addr) {
        if (addr.substr(0,2) == "0x") addr = addr.substr(2);
        int a = AddressSplitter::hexToInt(addr);
        int vpn = AddressSplitter::getVPN(a, shift);
        t.push_back({vpn, op=="W"});
    }

    cout << "Loaded " << t.size() << " accesses\n";
    return t;
}

// DATA STRUCTURES

struct PTE {
    bool valid=false;
    bool dirty=false;
    int frame=-1;
};

struct TLBEntry {
    int vpn=-1;
    int frame=-1;
    bool valid=false;
};

// STATS + LATENCY

class Stats {
public:
    int tlbHits=0, tlbMiss=0, pageHits=0, faults=0;
    int diskR=0, diskW=0, total=0;
};

class Latency {
public:
    long long time=0;

    void tlbHit(){ time+=1; }
    void tlbMiss(){ time+=10; }
    void fault(){ time+=10000000; }

    double EAT(int n){ return (double)time/n; }
};

// TLB

class TLB {
    vector<TLBEntry> e;
    int ptr=0;
public:
    TLB(int s){ e.resize(s); }

    int lookup(int vpn){
        for(auto &x:e)
            if(x.valid && x.vpn==vpn) return x.frame;
        return -1;
    }

    void insert(int vpn,int f){
        e[ptr]={vpn,f,true};
        ptr=(ptr+1)%e.size();
    }

    void invalidate(int vpn){
        for(auto &x:e)
            if(x.valid && x.vpn==vpn) x.valid=false;
    }
};

// LRU

class LRU {
    list<int> dq;
    unordered_map<int,list<int>::iterator> mp;
public:
    void use(int v){
        if(mp.count(v)) dq.erase(mp[v]);
        dq.push_front(v);
        mp[v]=dq.begin();
    }

    int evict(){
        int v=dq.back();
        dq.pop_back();
        mp.erase(v);
        return v;
    }
};

// CORE ENGINE

class Engine {
    unordered_map<int,PTE> pt;
    TLB tlb;
    queue<int> freeF;
    LRU lru;
    Stats stats;
    Latency lat;

public:
    Engine(int frames,int tlbSize):tlb(tlbSize){
        for(int i=0;i<frames;i++) freeF.push(i);
    }

    void access(int vpn,bool w){

        int f=tlb.lookup(vpn);

        if(f!=-1){
            stats.tlbHits++; stats.total++;
            lat.tlbHit();
            if(w) pt[vpn].dirty=true;
            lru.use(vpn);
            return;
        }

        stats.tlbMiss++; lat.tlbMiss();

        if(pt[vpn].valid){
            stats.pageHits++; stats.total++;
            tlb.insert(vpn,pt[vpn].frame);
            if(w) pt[vpn].dirty=true;
            lru.use(vpn);
            return;
        }

        // PAGE FAULT
        stats.faults++; stats.total++;
        stats.diskR++; lat.fault();

        int frame;

        if(!freeF.empty()){
            frame=freeF.front(); freeF.pop();
        }else{
            int victim=lru.evict();

            if(pt[victim].dirty) stats.diskW++;

            frame=pt[victim].frame;
            pt[victim].valid=false;
            tlb.invalidate(victim);
        }

        pt[vpn]={true,w,frame};
        tlb.insert(vpn,frame);
        lru.use(vpn);
    }

    void report(){
        cout<<"\n===== FINAL REPORT =====\n";
        cout<<"Accesses: "<<stats.total<<"\n";
        cout<<"TLB Hits: "<<stats.tlbHits<<"\n";
        cout<<"Page Faults: "<<stats.faults<<"\n";
        cout<<"Disk Reads: "<<stats.diskR<<"\n";
        cout<<"Disk Writes: "<<stats.diskW<<"\n";
        cout<<"EAT(ns): "<<lat.EAT(stats.total)<<"\n";
    }
};

// MAIN

int main(){

    Config c=ConfigParser::load("config.txt");

    Engine sim(c.numFrames,c.tlbSize);

    auto trace=loadTrace("trace.txt",c.offsetBits);

    for(auto &x:trace){
        sim.access(x.vpn,x.write);
    }

    sim.report();
}
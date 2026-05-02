Phase 1 — Startup: Load Configuration
Before anything runs, the system reads a config file (JSON/YAML/TXT) that defines:

Physical RAM size (e.g., 256 KB)
Page size (e.g., 4 KB)
TLB size (e.g., 16 entries)
Latency values (TLB = 1ns, RAM = 10ns, Disk = 10ms)

From these, the system computes how many physical frames exist (RAM / Page Size) and the bit-shift value (log2(Page Size)).

Phase 2 — Input: Read the Trace File
The trace file has 100,000+ lines, each like:
0x1A3F  R
0x2B10  W
Each line is a virtual address + operation (Read/Write). These are read line by line (never all at once) and fed into the MMU pipeline.

Phase 3 — Address Translation (MMU Logic)
Every virtual address gets split into two parts using bit operations:
Virtual Address = [ VPN | Offset ]

VPN    = address >> shift          (e.g., >> 12 for 4KB pages)
Offset = address & (pageSize - 1)  (e.g., & 0xFFF)
The VPN is what gets translated. The Offset stays unchanged and is appended to the final physical frame number to form the physical address.

Phase 4 — TLB Lookup (The Fast Path)
The TLB is a small cache (e.g., 16 entries) that stores recent VPN → Frame mappings.
Check TLB for VPN
  ├── TLB HIT, get frame directly (+1ns), done 
  └── TLB MISS, go to Page Table (+10ns extra)
On a hit, physical address = (frame << shift), offset. Fast and done.

Phase 5 — Page Table Lookup (on TLB Miss)
The Page Table is a dictionary/map of every VPN the process owns. Each entry (PTE) contains:

Valid bit — is this page in RAM right now?
Dirty bit — has this page been written to?
Frame number — where in physical RAM it lives

Check Page Table for VPN
  ├── Valid bit = 1 → PAGE HIT, get frame, update TLB 
  └── Valid bit = 0 → PAGE FAULT, must load from disk 

Phase 6 — Page Fault Handling (The Slow Path)
When a page isn't in RAM, the OS must load it from disk. This is expensive (+10ms). Steps:
1. Is there a free frame available?
   ├── YES → assign it directly
   └── NO  → run Replacement Algorithm to pick a VICTIM page
              ├── Check victim's Dirty bit
              │    └── If DIRTY → write to disk first (+10ms disk write)
              └── Evict victim, free the frame

2. Load new page from disk into the free frame (+10ms disk read)
3. Update Page Table (set valid=1, frame=X)
4. Update TLB with new mapping
5. Retry the memory access

Phase 7 — Replacement Algorithms (Who Gets Evicted?)
Three algorithms decide which page to kick out:
Algorithm Strategy Complexity FIFO Evict the oldest loaded page O(1) with a queue LRU Evict the least recently used O(1) with Doubly Linked List + Hash Map OPT Evict the page used farthest in the future Pre scan trace file

Phase 8 — Dirty Bit & Disk Write-Back

Every Write operation sets the page's dirty bit = 1
When a dirty page is evicted, it must be written back to disk (simulated as +10ms latency)
Clean pages can just be discarded (no disk write needed)


Phase 9 — Stats Engine & EAT Calculation
Throughout the simulation, counters track everything:

TLB Hits / Misses
Page Faults / Page Hits
Disk Reads / Disk Writes
Total simulated time (ns)

Effective Access Time (EAT):
EAT = (TLB_hit_rate × T_TLB)
    + (TLB_miss_rate × T_RAM)
    + (fault_rate × T_Disk)


#include "constants.h"

struct tlbEntry tlb[TLB_SIZE/TLB_ASSOC][TLB_ASSOC]; 
int pageTable[1024];
bool pageTableValid[1024];


void setupConstants() {

  //add tlb entries manually for now
  tlb[0][7].tag = 0x7A;
  tlb[0][7].ppn = 0x19;
  tlb[0][7].valid = true;
  tlb[1][2].tag = 0x46;
  tlb[1][2].ppn = 0x11;
  tlb[1][2].valid = true;
  
  //add page table entries
  pageTable[0xF4] = 0x15;
  pageTableValid[0xF4] = true;
  pageTable[0x51] = 0x12;
  pageTableValid[0x51] = true;
  pageTable[0x22] = 0xA3;
  pageTableValid[0x22] = true;
  pageTable[0x11] = 0x34;
  pageTableValid[0x11] = true;
  pageTable[0x8D] = 0x11;
  pageTableValid[0x8D] = true;
}

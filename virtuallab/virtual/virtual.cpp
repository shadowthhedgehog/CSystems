
#include<stdio.h> //printf
#include<math.h>  //log2
#include<stdlib.h>//strtol

#include "constants.h"

// int pageTable[1024] in constants header
// bool pageTableValid[1024] in constants header

/*define a tlb entry struct
struct tlbEntry {
  int tag;
  int ppn;
  bool valid = false;
};
*/

// pretty print TLB, included for help with debugging
void printTLB() {
  printf("TLB: \n");
  // print headers
  printf("%4s|", "Set");
  for (int set = 0; set < TLB_ASSOC; set++)
    printf("%3s %4s %5s|", "Tag", "PPN", "Valid");
  // print set
  printf("\n");
  for (int set = 0; set < TLB_SIZE/TLB_ASSOC; set++) {
    printf("%3d |", set);
    // print entries
    for (int entry = 0; entry < TLB_ASSOC; entry++) {
      printf(" %02x   %02x  %3d |", tlb[set][entry].tag,
          tlb[set][entry].ppn, tlb[set][entry].valid);
    }
    printf("\n");
  }
  printf("\n");
}

// print page table, can be used in debugging if desired
void printPageTable() {
  printf("Page Table:\nVPN  PPN Valid\n");
  for (int i = 0; i < 16; i++) {
    printf(" %02x | %02x %1d |\n", i, pageTable[i], pageTableValid[i]);
  }
  printf("\n");
}







// translates virtual address to physical address
int translate(int virtualAddress) {
  // TODO: implement address translation (insert code here)
  //bottom bits are VPO (virtual page offset)
  //top bits are VPN (virtual page number)
  //top bits are TLBT
  //bits to are TLBI
  int vpoBits, vpn, vpo, tlbiBits, tlbt, tlbi;
  int physicalAddress = 0;
  //check TLB
  int onesORzeros = 1;



//-------disassemble virtual address------------------
  vpoBits = log2(PAGE_SIZE) + 1;    //number of bits for VPO
//  vpnBits = VIRTUAL_WIDTH - vpoBits;

  vpn = virtualAddress >> vpoBits;  //vpn set as top bits

    //bottom vpoBits filled with 1
  for(int i = 0; i < vpoBits; i++){
    onesORzeros = (onesORzeros << 1) + 1;
  }
    //here onesORzeros's vpoBits should all be 1
  vpo = virtualAddress & onesORzeros;   //vpo set as bottom bits

  tlbiBits = log2(TLB_SIZE/TLB_ASSOC) + 1;
  tlbt = vpn >> tlbiBits;

  onesORzeros = 1;
  for(int i = 0; i < tlbiBits; i++){
    onesORzeros = (onesORzeros << 1) + 1;
  }
  tlbi = vpn & onesORzeros;


  printf("tlbt: %d tlbi: %d vpo: %d vpn: %d\n",tlbt,tlbi,vpo,vpn);
//----------done--------------------------------------


for(int i = 0; i < TLB_ASSOC; i++){
    if(tlb[tlbi][i].tag == tlbt){
        if(tlb[tlbi][i].valid == true){
            physicalAddress = (tlb[tlbi][i].ppn << vpoBits) | vpo;
            printf ("%s\n", "TLB HIT");
            return physicalAddress;
        }
    }
}


  printf("%s\n", "TLB MISS");

  if (pageTableValid[vpn] == 0){
    printf("%s\n", "PAGE FAULT");
    exit(0);
  }
  else
  physicalAddress = (pageTable[vpn] << vpoBits) | vpo;
  return physicalAddress;
}







// main function
int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("requries virtual address in HEX as argument!\n");
    return 0;
  }
  // set up TLB and page table (in constants.cpp)
  setupConstants();

  //take input address and parse it as a hexadecimal number
  int inputAddress = (int)strtol(argv[1], NULL, 16);

  printTLB();

  printPageTable();






  //translate to a physical address and print results
  printf("Physical address: 0x%x \n", translate(inputAddress));

  return 0;
}

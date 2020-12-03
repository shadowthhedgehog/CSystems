
#include<stdio.h>   //printf
#include<math.h>    //log2
#include<stdlib.h>  //strtol

#include "constants.h"


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






//----------------------Extract bits helper function---------------------------------------------
unsigned bits(unsigned input, unsigned topBit, unsigned bottomBit){
  unsigned mask = 0, output = 0;
  for (unsigned i = bottomBit; i <= topBit; i++){
    mask |= 1 << i;     //make 1's in desired bit spots
  }

  output = (input & mask) >> bottomBit;
  return output;
}
//------------------------------------------------------------------------------------------------






//-----------------------------Translate----------------------------------------------------------
// translates virtual address to physical address
int translate(int virtualAddress) {
    int vpoBits, vpn, vpo, tlbiBits, tlbt, tlbi;  //the pieces we break virtualAddress into
    int physicalAddress = 0;


  //-------disassemble virtual address------------------
    vpoBits = log2(PAGE_SIZE);    //number of bits for VPO
  //  vpnBits = VIRTUAL_WIDTH - vpoBits;

  //  vpn = virtualAddress >> vpoBits;
    vpn = bits(virtualAddress,VIRTUAL_WIDTH-1,vpoBits);     //vpn set as top bits
    vpo = bits(virtualAddress,vpoBits-1, 0);   //vpo set as bottom bits

    tlbiBits = log2(TLB_SIZE/TLB_ASSOC);    //number of bits for tlbi

    tlbt = bits(virtualAddress,VIRTUAL_WIDTH-1,(vpoBits+tlbiBits));   //tlbt is top of vpn
    tlbi = bits(virtualAddress,(vpoBits+tlbiBits-1),vpoBits);         //tlbi is bottom of vpn


    //the next line is just here for debugging purposes   ¯\_(ツ)_/¯
    //printf("vpoBits: %d tlbiBits: tlbt: %d tlbi: %d vpo: %d vpn: %d\n",vpoBits, tlbiBits, tlbt,tlbi,vpo,vpn);
  //----------done disassembling------------------------


      //check the tlb @ indexed line for tag
  for(int i = 0; i < TLB_ASSOC; i++){
      if(tlb[tlbi][i].tag == tlbt){
          if(tlb[tlbi][i].valid == true){
              physicalAddress = (tlb[tlbi][i].ppn << vpoBits) | vpo;
              printf ("%s\n", "TLB HIT");
              return physicalAddress;       //if there, print hit and return the physical address
          }
      }
  }


    // else, it wasn't in there :(
    printf("%s\n", "TLB MISS");
    // ..so lets check the page table!
    if (pageTableValid[vpn] == 0){
      printf("%s\n", "PAGE FAULT");
      exit(0);    // if it's not there, not much we can do but just die :(
    }
    else      //if it is there, send that puppy out!! :)
    physicalAddress = (pageTable[vpn] << vpoBits) | vpo;
    return physicalAddress;
}
//----------------------End Translate-------------------------------------------------------------






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

  //translate to a physical address and print results
  printf("Physical address: 0x%x \n", translate(inputAddress));

  return 0;
}

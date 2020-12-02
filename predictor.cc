#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <string.h>
#include <inttypes.h>
#include <iostream>
#include <math.h>

using namespace std;

#include "cbp3_def.h"
#include "cbp3_framework.h"

// rewind_marked is also used to show how to rewind the reader for multiple runs.
// the predictor will use saturating-counter in the first run,
// the gselect predictor will be used in the second run, and
// the gshare predictor will be used in the third run.

// two branch history registers:
// the framework provids real branch results at fetch stage to simplify branch history
// update for predicting later branches. however, they are not available until execution stage
// in a real machine. therefore, you can only use them to update predictors at or after the
// branch is executed.

// cost: depending on predictor size

//typedef enum {StrNotTaken, WkNotTaken, WkTaken, StrTaken} state;


bool gpred = true;
uint16_t TwoBSetTable[4096] = {0};
uint16_t GShareSetTable[8192] = {0};
uint16_t GSelectSetTable[8192] = {0};
uint16_t HashFunc(uint32_t addr, int TableSize){		//hash function
	uint16_t ret;
	ret = floor(TableSize * (fmod(addr * .618033 , 1)));
	return ret;
}

void decBinary(int arr[], int n) 		//convert int to decimal
{ 
    int k = log2(n); 
    while (n > 0) { 
        arr[k--] = n % 2; 
        n /= 2; 
    } 
} 
 
int binaryDec(int arr[], int n) 		//convert decimal to int
{ 
    int ans = 0; 
    for (int i = 0; i < n; i++) 
        ans += arr[i] << (n - i - 1); 
    return ans; 
}



uint32_t brh_fetch;
uint32_t brh_retire;

// count number of runs
uint32_t runs;

void PredictorInit() {
    runs = 0;
}

void PredictorReset() {
    // this function is called before EVERY run
    // it is used to reset predictors and change configurations

    if (runs == 0)
        printf("Predictor: 2-bit saturating-counter\n");
    else if (runs == 1)
        printf("Predictor: gseletct\n");
    else if (runs == 2)
        printf("Predictor: gshare\n");

    brh_fetch = 0;
    brh_retire = 0;
}

void PredictorRunACycle() {
    // get info about what uops are processed at each pipeline stage
    const cbp3_cycle_activity_t *cycle_info = get_cycle_info();


	
    // make prediction at fetch stage
    for (int i = 0; i < cycle_info->num_fetch; i++) {
        uint32_t fe_ptr = cycle_info->fetch_q[i];
        const cbp3_uop_dynamic_t *uop = &fetch_entry(fe_ptr)->uop;

        if (!(uop->type & IS_BR_CONDITIONAL)) continue;
        if (runs == 0) {
			
			
//	------------------------------------my 2 bit counter code---------------------------------------------------------------			
		
		uint16_t index = HashFunc(uop->pc, 4096);		//hash address
		
					//make prediction
			switch(TwoBSetTable[index]){
				case 1:{
					gpred = false;					
					break;
				}
				case 2:{
					gpred = false;					
					break;
				}
				case 3:{
					gpred = true;
					break;
				}
				case 4:{
					gpred = true;
					break;
				}
				default: break;
			}


			//update table based on taken or not
		if (TwoBSetTable[index] == 0){			//if not in table, add addrress to list
				TwoBSetTable[index] = 3;			//insert as weak taken by default
			}
			if(uop->br_taken == true){		//if actually taken, each status shifts toward strongly taken
				switch(TwoBSetTable[index]){
					case 1:{
						TwoBSetTable[index] = 2;					
						break;
					}
					case 2:{
						TwoBSetTable[index] = 3;					
						break;
					}
					case 3:{
						TwoBSetTable[index] = 4;
						break;
					}
					case 4:{
						TwoBSetTable[index] = 4;
						break;
					}
					default: break;
				}			
			}
			else {								//if actually not taken, each state shifts toward strongly not taken
				switch(TwoBSetTable[index]){
					case 1:{
						TwoBSetTable[index] = 1;					
						break;
					}
					case 2:{
						TwoBSetTable[index] = 1;					
						break;
					}
					case 3:{
						TwoBSetTable[index] = 2;
						break;
					}
					case 4:{
						TwoBSetTable[index] = 3;
						break;
					}
					default: break;
				}			
			}
				
            //report guess
            assert(report_pred(fe_ptr, false, gpred));

			
//	-----------------------end of my 2 bit counter code---------------------------------------------------------			

        } else if (runs == 1) {
	  
//----------------------------my gselect code------------------------------------------------------------------	  
	  
	  //index = { {branch_addr[31:16]}, {branch_history[15:0]} }
	  
	  int a1[32], a2[32], a3[32];
	  decBinary(a1, uop->pc); 		 //a1 is binary of address
	  decBinary(a2, brh_fetch);		 //a2 is binary of branch history (brh_fetch)
	  for(int i = 0; i < 16; i++){   //a3[15:0] is history[15:0]
		a3[i] = a2[i];
	  }
	  for(int i = 16; i < 32; i++){   //a3[32:16] is address[32:16}
		  a3[i] = a1[i-16];
	  }
	  int temp = binaryDec(a3, 32);  //concatenated bottom 16 bits of history and address
	  uint16_t index = HashFunc(temp, 8192);	   //run that thorough hash func
	  





					//make prediction
			switch(GSelectSetTable[index]){
				case 1:{
					gpred = false;					
					break;
				}
				case 2:{
					gpred = false;					
					break;
				}
				case 3:{
					gpred = true;
					break;
				}
				case 4:{
					gpred = true;
					break;
				}
				default: break;
			}


			//update table based on taken or not
		if (GSelectSetTable[index] == 0){			//if not in table, add addrress to list
				GSelectSetTable[index] = 3;			//insert as weak taken by default
			}
			if(uop->br_taken == true){		//if actually taken, each status shifts toward strongly taken
				switch(GSelectSetTable[index]){
					case 1:{
						GSelectSetTable[index] = 2;			
						
						break;
					}
					case 2:{
						GSelectSetTable[index] = 3;					
						break;
					}
					case 3:{
						GSelectSetTable[index] = 4;
						break;
					}
					case 4:{
						GSelectSetTable[index] = 4;
						break;
					}
					default: break;
				}			
			}
			else {								//if actually not taken, each state shifts toward strongly not taken
				switch(GSelectSetTable[index]){
					case 1:{
						GSelectSetTable[index] = 1;					
						break;
					}
					case 2:{
						GSelectSetTable[index] = 1;					
						break;
					}
					case 3:{
						GSelectSetTable[index] = 2;
						break;
					}
					case 4:{
						GSelectSetTable[index] = 3;
						break;
					}
					default: break;
				}			
			}

          
			

            
            assert(report_pred(fe_ptr, false, gpred));
	    
//------------------------------end of my gselect code---------------------------------------------------------	    

        } else if (runs == 2) {
			
//----------------------------my gshare code------------------------------------------------------------------				
	  
	  // index = branch_addr XOR branch_history
		uint32_t index = fmod((uop->pc ^ brh_fetch), 8192);


//	  uint16_t index = HashFunc(temp, 1024);	   //run that thorough hash func
	  


					//make prediction
			switch(GShareSetTable[index]){
				case 1:{
					gpred = false;					
					break;
				}
				case 2:{
					gpred = false;					
					break;
				}
				case 3:{
					gpred = true;
					break;
				}
				case 4:{
					gpred = true;
					break;
				}
				default: break;
			}


			//update table based on taken or not
		if (GShareSetTable[index] == 0){			//if not in table, add addrress to list
				GShareSetTable[index] = 3;			//insert as weak taken by default
			}
			if(uop->br_taken == true){		//if actually taken, each status shifts toward strongly taken
				switch(GShareSetTable[index]){
					case 1:{
						GShareSetTable[index] = 2;			
						
						break;
					}
					case 2:{
						GShareSetTable[index] = 3;					
						break;
					}
					case 3:{
						GShareSetTable[index] = 4;
						break;
					}
					case 4:{
						GShareSetTable[index] = 4;
						break;
					}
					default: break;
				}			
			}
			else {								//if actually not taken, each state shifts toward strongly not taken
				switch(GShareSetTable[index]){
					case 1:{
						GShareSetTable[index] = 1;					
						break;
					}
					case 2:{
						GShareSetTable[index] = 1;					
						break;
					}
					case 3:{
						GShareSetTable[index] = 2;
						break;
					}
					case 4:{
						GShareSetTable[index] = 3;
						break;
					}
					default: break;
				}			
			}

         
            assert(report_pred(fe_ptr, false, gpred));

        }
			brh_fetch = ((brh_fetch << 1) | uop->br_taken);		//shift actual result into history
        // update fetch branch history
		
//------------------------------end of my gshare code---------------------------------------------------------		
    }

    for (int i = 0; i < cycle_info->num_retire; i++) {
        uint32_t rob_ptr = cycle_info->retire_q[i];
        const cbp3_uop_dynamic_t *uop = &rob_entry(rob_ptr)->uop;

        if(!(uop->type & IS_BR_CONDITIONAL)) continue;
        if (runs == 0) {
					
			
			
			
        } else if (runs == 1) {
            // update gselect predictor
        } else if (runs == 2) {
            // update gshare predictor
        }

        // update retire branch history
    }
}

void PredictorRunEnd() {
    runs ++;
    if (runs < 3) // set rewind_marked to indicate that we want more runs
        rewind_marked = true;
}

void PredictorExit() {
}

#include "prefetcher.h"
#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>

Prefetcher::Prefetcher() { 
	_ready=false;
	rpt.resize(TABLE_SIZE);
	hit_count=0;
}

bool Prefetcher::hasRequest(u_int32_t cycle) {  
	if (!_ready){ // if we do not have a stride hit, check the prefetch buffer for any potential addresses queued up
		if(pb[cycle%BUFFER_SIZE!=0]){
			_nextReq.addr = pb[cycle%BUFFER_SIZE];
			 pb[cycle%BUFFER_SIZE]=0;
			_ready = true;
		}	
	}
	return _ready;		
} 


Request Prefetcher::getRequest(u_int32_t cycle) {
		return _nextReq;
}

void Prefetcher::completeRequest(u_int32_t cycle) {
	_ready = false; 
}



void Prefetcher::cpuRequest(Request req) {  
	bool hit= false;
	if(!req.HitL1 && req.load) {
		u_int32_t max_LRU = 0; //used to keep track of which prediction table entry to remove in case of a miss
		u_int32_t index = 0;
		for(int i=0; i<TABLE_SIZE;i++){	// check the prediction table for any hits
			if(req.pc==rpt[i].tag){ // table instructions are tagged by the PC
				hit=true;
				hit_count++;
				u_int32_t stride= req.addr-rpt[i].addr;
				if(stride==rpt[i].stride){ // if the offset of the current address matches the history issue a prefetch following the hsitory 
					_nextReq.addr=req.addr+rpt[i].stride;
					_ready=true;
				}
				else{
					_nextReq.addr=req.addr+stride; // if the offset does not match issue a prefetch with the new offset 
					_ready=true;
				}
				rpt[i].addr=req.addr; // update most recent hit address 
				rpt[i].LRU=0; // reset LRU flag to 0 since this entry was found 
				break;
			}
			else{
				rpt[i].LRU++; // we did not get a hit at entry "i" so update the LRU count 
				if (rpt[i].LRU > max_LRU){ 
					max_LRU = rpt[i].LRU;
					index = i;
				}
			}
		}
		if (!hit){ // if we do not get a hit in the table, create a new entry and replace the least recently used one with it. 
			RPT rpt_entry; //  new entry 
			rpt_entry.addr= req.addr;
			rpt_entry.tag= req.pc;
			rpt_entry.stride=STRIDE;
			rpt_entry.LRU=0;
			rpt[index]=rpt_entry;
		}
	}
	if(!_ready && hit_count>HIT_THRESH){ 
		_nextReq.addr = req.addr + STRIDE;
		_ready = true; 
		for (int j = 0; j < BUFFER_SIZE; j++){
			pb[j] = _nextReq.addr + (STRIDE*(j+2)); //allocate a stream buffer with the next 5 cahcelines 
		}
	} 	
}

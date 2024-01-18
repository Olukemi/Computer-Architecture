#ifndef PREFETCHER_H
#define PREFETCHER_H

#define STRIDE 16
#define BUFFER_SIZE 10
#define TABLE_SIZE 128
#define HIT_THRESH 100
#include "mem-sim.h"
#include <vector>

struct RPT{ // prediction table entry 
	u_int32_t stride;
	u_int32_t addr;
	u_int32_t tag;
	u_int32_t LRU;
};

class Prefetcher {
	
  private:
	bool _ready; // prefetch flag 
	Request _nextReq; // next predicted prefetch request 
	u_int32_t pb[BUFFER_SIZE]={}; // prefetch buffer of size 10 (4B*10= 40 B of memory)
	std::vector<RPT> rpt; // reference prediction table of size 128(16B per row x 128 rows= 2KB of memory saved )
	int hit_count;
	

  public:
	Prefetcher();

	// should return true if a request is ready for this cycle
	bool hasRequest(u_int32_t cycle);

	// request a desired address be brought in
	Request getRequest(u_int32_t cycle);
	\
	// this function is called whenever the last prefetcher request was successfully sent to the L2
	void completeRequest(u_int32_t cycle);
	/*
	 * This function is called whenever the CPU references memory.
	 * Note that only the addr, pc, load, issuedAt, and HitL1 should be considered valid data
	 */
	void cpuRequest(Request req); 
	// void stateMachine(int state);
};

#endif

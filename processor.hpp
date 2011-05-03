#include <stdio.h>
#include <engine/cyclesim.hpp>
#include "mdata.h"

class Processor : public Module {
public:
	struct {
		Port<mdata_t> data;
		ControlPort busy;
	} in;

	struct {
		Port<mdata_t> data;
	} out;

	Processor(int id, char *file);

    virtual void Reset();
    virtual void Start();
    virtual void End();
    virtual void print_hook(void) const;

private:
	FILE *trace;
	int wait;
	unsigned long wait_addr;
	cycles_t total_latency_cycles;
	cycles_t start_cycle;
	cycles_t end_cycle;
	unsigned long requests;
	int pid;
};


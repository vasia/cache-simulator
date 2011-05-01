#include "processor.hpp"
#include <stdio.h>

//initialization function - open trace file
Processor::Processor(char *file) : Module("Processor", address_t(0), 0)
{
	trace = fopen(file,"r");

	out.data.setModule(this);
	in.data.setModule(this);
	in.busy.setModule(this);
}

void Processor::Reset()
{
	wait = 0;
	out.data.sendNothing();
	rewind(trace);
}

void Processor::Start()
{
	mdata_t item; //mdata_t defined in mdata.h

	//if I'm not waiting and the input port is not busy
	//read an item from the file
	if (!wait && !in.busy) {
		fscanf(trace, "%c %lu\n", &item.type, &item.addr);
		item.size = 4;

		printf("P: item = %c %lu\n", item.type, item.addr);

		//send the item to the output port
		out.data = item;

		//if the instruction is a load
		//The processor has to wait both for reads and writes
		//set the waiting flag
		wait = 1;
		wait_addr = item.addr;

		printf("P: wait = %d, wait_addr = %lu\n", wait, wait_addr);

	}
}

void Processor::End()
{
	mdata_t item;

	//if there is data in the input port
	if (!in.data.isNothing()) {
		item = in.data;

		//and it's not the data I'm waiting for
		if (item.addr != wait_addr) {
			fprintf(stderr, "P: ERROR: received wrong address\n");
		}
		//data received -> don't wait anymore
		wait = 0;
	}

	//if the trace is over and I'm not waiting for anything
	if (feof(trace) && !wait) { 
		setNextCycle(NEVER);
	//if I'm not waiting and I'm not busy
	} else if (!wait && !in.busy) {
		setNextCycle(Sim::cycle + cycles_t(1));
	} else {
		setNextCycle(NEVER);
	}
}

void Processor::print_hook(void) const {}

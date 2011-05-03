#include "processor.hpp"
#include <stdio.h>

//initialization function - open trace file
Processor::Processor(int id, char *file) : Module("Processor", address_t(0), 0)
{
	trace = fopen(file,"r");

	out.data.setModule(this);
	in.data.setModule(this);
	in.busy.setModule(this);

	pid = id;
}

void Processor::Reset()
{
	wait = 0;
	out.data.sendNothing();
	rewind(trace);
	
	//statistics variables
	total_latency_cycles = cycles_t(0);
	start_cycle = cycles_t(0);
	end_cycle = cycles_t(0);
	requests = 0;
}

void Processor::Start()
{
	mdata_t item; //mdata_t defined in mdata.h

	//if I'm not waiting and the input port is not busy
	//read an item from the file
	if (!wait && !in.busy) {
		fscanf(trace, "%c %lu\n", &item.type, &item.addr);
		//to make sure we don't read rubbish from the file
		if(item.type == 'R' || item.type == 'W'){ 
			item.size = 4;

			printf("P(%d): item = %c %lu\n", pid, item.type, item.addr);

			//send the item to the output port
			out.data = item;

			//if the instruction is a load
			if(item.type == 'R'){
				wait = 1;
				wait_addr = item.addr;
				printf("P(%d): wait = %d, wait_addr = %lu\n", pid, wait, wait_addr);

				//counting statistics for read requests
				//not considering the time while cache is busy
				start_cycle = Sim::cycle;
				requests++;
			}
		}
	}
}

void Processor::End()
{
	mdata_t item;

	//if there is data in the input port
	if (!in.data.isNothing()) {
		item = in.data;
		printf("P(%d): received item = %c %lu\n", pid, item.type, item.addr);

		//and it's not the data I'm waiting for
		if (item.addr != wait_addr) {
			fprintf(stderr, "P(%d): ERROR: received wrong address\n", pid);
		}
		//data received -> don't wait anymore
		wait = 0;

		//calculate the latency
		end_cycle = Sim::cycle;
		total_latency_cycles = total_latency_cycles + (end_cycle - start_cycle);
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

void Processor::print_hook(void) const {
	//printing statistics
	std::cout << "P(" << pid << "): ## Requests: " << requests << "\n";
	std::cout << "P(" << pid << "): ## Total latency cycles: " << total_latency_cycles << "\n";
	//the following print doesn't give a decimal value :(
	std::cout << "P(" << pid << "): ## Average latency: " << total_latency_cycles/cycles_t(requests) << "\n";
}

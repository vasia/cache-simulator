#include "cache.hpp"
#include <stdio.h>

Cache::Cache(int delay, int lines, int line_size) : Module("Cache", address_t(0), 0)
{
	latency = delay;
	clines = lines;
	cline_size = line_size;

	out.data.setModule(this);
	out.busy.setModule(this);
	in.data.setModule(this);
	in.busy.setModule(this);
	
	//allocate buffers
	buffer = new unsigned long[clines];

	dirty_bit_buff = new char[clines];
}

void Cache::Reset()
{
	respond = NEVER;
	out.data.sendNothing();
	out.busy = false;
	in.busy = false;
	wait = 0;
}

//send data in the start of the cycle
void Cache::Start()
{
	mdata_t item;

	if (Sim::cycle >= respond) {
		item.type = 'W';
		item.addr = respond_addr;
		printf("C(s): sending %lu\n", respond_addr);
		out.data = item;
		respond = NEVER;
	}
}

//read data in the end of the cycle
void Cache::End()
{
	mdata_t item;

	if (!wait && !in.busy) {
		if (!in.data.isNothing()) {
			item = in.data;

	printf("C: item = %c %lu\n", item.type, item.addr);

			if (item.type == 'R') {
				printf("C: is load!\n");
				//check if data is in the cache
				//if(data_in_the_cache){
				
				respond = Sim::cycle + cycles_t(latency);
				//}
				//else{
				//respond = Sim::cycle + cycles_t(latency);
				//}
				respond_addr = item.addr;
	std::cout << "C: respond = " << respond << "\n";
			}
		}
	}

	if (respond != NEVER) {
		out.busy = true;
	} else {
		out.busy = false;
	}

	setNextCycle(respond);
}

void Cache::print_hook(void) const {}


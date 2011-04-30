#include "cache.hpp"
#include <stdio.h>

Cache::Cache(int delay, int lines, int line_size) : Module("Cache", address_t(0), 0)
{
	latency = delay;
	clines = lines;
	cline_size = line_size;

	p_out.data.setModule(this);
	p_out.busy.setModule(this);
	p_in.data.setModule(this);
	p_in.busy.setModule(this);

	m_out.data.setModule(this);
	m_out.busy.setModule(this);
	m_in.data.setModule(this);
	m_in.busy.setModule(this);

	
	//allocate buffers
	buffer = new unsigned long[clines];
	dirty_bit_buff = new char[clines];

	//set all dirty bits to 0
	int i;
	for(i=0; i<clines; i++)
		dirty_bit_buff[i] = 0;
}

void Cache::Reset()
{
	respond = NEVER;
	p_out.data.sendNothing();
	p_out.busy = false;
	p_in.busy = false;
	m_out.data.sendNothing();
	m_out.busy = false;
	m_in.busy = false;
	wait = 0;
}

//send data in the start of the cycle
void Cache::Start()
{
	mdata_t item;

	//have to somehow distinguish between sending data to the memory or the processor

	//send data to the processor
	if (Sim::cycle >= respond) {
		item.type = 'W';
		item.addr = respond_addr;
		printf("C(s): sending %lu\n", respond_addr);
		p_out.data = item;
		respond = NEVER;
	}

	//send data to memory
	/*if (!wait && !in.busy) {
		fscanf(trace, "%c %lx", &item.type, &item.addr);
		item.size = 4;

		printf("C: item = %c %lu\n", item.type, item.addr);

		//send the item to the output port
		out.data = item;

		//if the instruction is a load
		if (item.type == 'R') {
			//set the waiting flag
			wait = 1;
			wait_addr = item.addr;
		}

		printf("C: wait = %d, wait_addr = %lu\n", wait, wait_addr);

	}*/
}

//read data in the end of the cycle
void Cache::End()
{
	mdata_t item;
	int pos;	//mapped position of the item in the cache

	//read from processor
	if (!wait && !p_in.busy) {
		if (!p_in.data.isNothing()) {
			item = p_in.data;

			printf("C: item = %c %lu\n", item.type, item.addr);

			if (item.type == 'R') {	//same for writes???
				printf("C: is load!\n");

				//check if data is in the cache
				//direct-mapped cache: pos = (addr/clines)mod cline_size
				pos = (int)((item.addr/clines)%cline_size);
				
				if(buffer[pos] <= item.addr && item.addr < buffer[pos]+cline_size){
					//data is in the cache
					//if READ -> just set respond
					respond = Sim::cycle + cycles_t(latency);
					//if WRITE -> also set dirty bit
					
				}
				else{
				//data not in the cache
				//check dirty bit for write-back policy
				
				//respond = Sim::cycle + cycles_t(latency);
				}
				respond_addr = item.addr;
				std::cout << "C: respond = " << respond << "\n";
			}

		}
	}

	//read from memory
	if (!wait && !m_in.busy) {;}

	if (respond != NEVER) {
		p_out.busy = true;
	} else {
		p_out.busy = false;
	}

	setNextCycle(respond);
}

void Cache::print_hook(void) const {}


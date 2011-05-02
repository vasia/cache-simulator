#include "cache.hpp"
#include <stdio.h>

Cache::Cache(int delay, int lines, int line_size, unsigned long *cache_buffer, char *cache_dirty_bit_buffer) : Module("Cache", address_t(0), 0)
{
	latency = delay;
	clines = lines;
	cline_size = line_size;
	buffer = cache_buffer;
	dirty_bit_buffer = cache_dirty_bit_buffer;

	p_out.data.setModule(this);
	p_out.busy.setModule(this);
	p_in.data.setModule(this);

	m_out.data.setModule(this);
	m_in.data.setModule(this);
	m_in.busy.setModule(this);

}

void Cache::Reset()
{
	respond = NEVER;
	p_out.data.sendNothing();
	p_out.busy = false;
	m_out.data.sendNothing();
	wait = 0;
	miss = 0;
	write_back = 0;

	//set all dirty bits to 0
	int i;
	for(i=0; i<clines; i++){
		dirty_bit_buffer[i] = '0';
		//buffer[i] = -cline_size;
		buffer[i] = i*cline_size;
	}

	hits = 0;
	misses = 0;
}

//send data in the start of the cycle
void Cache::Start()
{
	mdata_t item;
	
	//send data to the processor
	if (Sim::cycle >= respond) {
		item.type = 'W';	//doesn't make sense (before you ask)
		item.addr = respond_addr;
		printf("C(s): sending %lu\n", respond_addr);
		p_out.data = item;
		respond = NEVER;
	}

	//send data to the memory
	else if (miss && !wait && !m_in.busy && miss_item.type == 'R') {
		printf("C: miss item = %c %lu\n", miss_item.type, miss_item.addr);
		//send the item to the output port
		m_out.data = miss_item;
		wait = 1;
		wait_addr = miss_item.addr;
		printf("C: wait = %d, wait_addr = %lu\n", wait, wait_addr);
	}
	
}

//read data in the end of the cycle
void Cache::End()
{
	mdata_t item;
	int pos;

	//if the memory has sent data
	if (!m_in.data.isNothing()) {
		item = m_in.data;
		printf("C: received item = %c %lu\n", item.type, item.addr);

		//and it's not the data I'm waiting for
		if (item.addr != wait_addr) {
			fprintf(stderr, "C: ERROR: received wrong address\n");
		}
		//data received -> don't wait anymore
		wait = 0;
		miss = 0;

		//check dirty bit

		//update the cache contents
		pos = (int)((item.addr/cline_size)%clines);	
		buffer[pos] = (item.addr/cline_size)*cline_size;
		printf("C: Cache updated at position %d with address %lu\n", pos, buffer[pos]);

		//now I have the data and I can reply to the processor
		respond = Sim::cycle + cycles_t(latency);
		respond_addr = item.addr;
		std::cout << "C: respond = " << respond << "\n";
	}

	//the processor sent a request to the cache
	else if (!p_in.data.isNothing()) {
		item = p_in.data;

		printf("C: item = %c %lu\n", item.type, item.addr);

		if (item.type == 'R') {
			printf("C: is read!\n");
			//check if it is a hit
			pos = (int)((item.addr/cline_size)%clines);	
			if(buffer[pos] <= item.addr && item.addr < buffer[pos]+cline_size){
				// HIT //
				hits++;
				printf("C(s): It's a hit! %lu\n", item.addr);		
				respond = Sim::cycle + cycles_t(latency);
				respond_addr = item.addr;
				std::cout << "C: respond = " << respond << "\n";
			}
			else{
				// MISS //
				misses++;
				printf("C: It's a miss! %lu\n", item.addr);
				miss = 1;
				miss_item = item;
			}
		}
	}

	if ((respond != NEVER) || miss) {
		p_out.busy = true;
	} else {
		p_out.busy = false;
	}

	if(miss){
		setNextCycle(Sim::cycle + cycles_t(1));
	}
	else{
		setNextCycle(respond);
	}
}

void Cache::print_hook(void) const {
	printf("\n\n Hits = %d \n Misses = %d\n\n", hits, misses);
}

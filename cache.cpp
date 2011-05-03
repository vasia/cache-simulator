#include "cache.hpp"
#include <stdio.h>

Cache::Cache(int id, int delay, int lines, int line_size, unsigned long *cache_buffer, char *cache_dirty_bit_buffer) : Module("Cache", address_t(0), 0)
{
	latency = delay;
	clines = lines;
	cline_size = line_size;
	buffer = cache_buffer;
	dirty_bit_buffer = cache_dirty_bit_buffer;
	cid = id;

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
		buffer[i] = -cline_size;
	}
	
	hits = 0;
	misses = 0;
}

//send data in the start of the cycle
void Cache::Start()
{
	mdata_t item;
	
	//send data to the processor
	if(Sim::cycle >= respond){
		item.type = 'R';	//does it make sense now? (before you ask)
		item.addr = respond_addr;
		printf("C(%d): sending %lu\n", cid, respond_addr);
		p_out.data = item;
		respond = NEVER;
	}

	//send data to the memory (read miss case)
	else if(miss && !wait && !m_in.busy && miss_item.type == 'R'){
		printf("C(%d): miss item = %c %lu\n", cid, miss_item.type, miss_item.addr);
		//send the item to the output port
		m_out.data = miss_item;
		wait = 1;
		wait_addr = miss_item.addr;
		printf("C(%d): wait = %d, wait_addr = %lu\n", cid, wait, wait_addr);
	}

	//send data to memory (write miss case)
	else if(miss && !wait && !m_in.busy && miss_item.type == 'W'){
		printf("C(%d): miss item = %c %lu\n", cid, miss_item.type, miss_item.addr);
		//send the item to the output port
		item = miss_item;
		//setting the type to 'R' so that the memory will send a response
		item.type = 'R';
		m_out.data = item;
		
		wait = 1;
		wait_addr = miss_item.addr;
		printf("C(%d): wait = %d, wait_addr = %lu\n", cid, wait, wait_addr);
	}

	//send data to the memory (replacing a dirty block)
	else if(write_back && !wait && !m_in.busy){
		printf("C(%d): sending dirty item %c %lu to memory\n", cid, dirty_item.type, dirty_item.addr);
		//send item to the output port
		m_out.data = dirty_item;
		write_back = 0;
		//set the respond, so we can reply to the processor
		respond = Sim::cycle + cycles_t(latency);
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
		printf("C(%d): received item = %c %lu\n", cid, item.type, item.addr);

		//and it's not the data I'm waiting for
		if (item.addr != wait_addr) {
			fprintf(stderr, "C(%d): ERROR: received wrong address\n", cid);
		}
		//data received -> don't wait anymore
		wait = 0;
		miss = 0;
		pos = (int)((item.addr/cline_size)%clines);	

		//check dirty bit
		if(dirty_bit_buffer[pos] == '1'){
			printf("C(%d): Dirty block!\n", cid); 
			//need to replace a dirty block
			//so, we need to send it back to memory (write-back policy)
			dirty_item.addr = buffer[pos];
			dirty_item.type = 'W';
			dirty_item.size = 4;
			write_back = 1;					
		}
		else{
			//update the cache contents
			buffer[pos] = (item.addr/cline_size)*cline_size;
			printf("C(%d): Cache updated at position %d with address %lu\n", cid, pos, buffer[pos]);

			//reset dirty bit
			dirty_bit_buffer[pos] = '0';

			if(miss_item.type == 'R'){
				//now I have the data and I can reply to the processor
				respond = Sim::cycle + cycles_t(latency);
				respond_addr = item.addr;
				std::cout << "C(" << cid << "): respond = " << respond << "\n";
			}
		}
	}

	//the processor sent a request to the cache
	else if (!p_in.data.isNothing()) {
		item = p_in.data;

		printf("C(%d): item = %c %lu\n", cid, item.type, item.addr);

		if (item.type == 'R') {
			printf("C(%d): is read!\n", cid);
			//check if it is a hit
			pos = (int)((item.addr/cline_size)%clines);	
			if(buffer[pos] <= item.addr && item.addr < buffer[pos]+cline_size){
				// HIT //
				hits++;
				printf("C(%d): It's a read hit! %lu\n", cid, item.addr);		
				respond = Sim::cycle + cycles_t(latency);
				respond_addr = item.addr;
				std::cout << "C(" << cid << "): respond = " << respond << "\n";
			}
			else{
				// MISS //
				misses++;
				printf("C(%d): It's a read miss! %lu\n", cid, item.addr);
				miss = 1;
				miss_item = item;
				//only setting the respond address because we don't know how long it will take the memory to answer
				respond_addr = item.addr;
			}
		}
		else if(item.type == 'W'){
			printf("C(%d): it's a write!\n", cid);
			//check if it is a hit
			pos = (int)((item.addr/cline_size)%clines);	
			if(buffer[pos] <= item.addr && item.addr < buffer[pos]+cline_size){
				// HIT //
				hits++;
				printf("C(%d): It's a write hit! %lu\n", cid, item.addr);		
				dirty_bit_buffer[pos] = '1';
			}
			else{
				// MISS //
				misses++;
				printf("C(%d): It's a write miss! %lu\n", cid, item.addr);
				miss = 1;
				miss_item = item;
				//it's a write miss -> no need to respond to processor -> we don't set the respond_addr
			}

		}
	}

	if ((respond != NEVER) || miss || write_back) {
		p_out.busy = true;
	} else {
		p_out.busy = false;
	}

	if(miss || write_back){
		setNextCycle(Sim::cycle + cycles_t(1));
	}
	else{
		setNextCycle(respond);
	}
}

void Cache::print_hook(void) const {
	printf("\n\n C(%d): Hits = %d \n Misses = %d\n\n", cid, hits, misses);
}

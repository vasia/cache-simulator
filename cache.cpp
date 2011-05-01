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
	//p_in.busy.setModule(this);

	m_out.data.setModule(this);
	//m_out.busy.setModule(this);
	m_in.data.setModule(this);
	m_in.busy.setModule(this);

}

void Cache::Reset()
{
	respond = NEVER;
	p_out.data.sendNothing();
	p_out.busy = false;
	//p_in.busy = false;
	m_out.data.sendNothing();
	//m_out.busy = false;
	//m_in.busy = false;
	wait = 0;
	miss = 0;
	dirty = 0;

	//set all dirty bits to 0
	int i;
	for(i=0; i<clines; i++){
		dirty_bit_buffer[i] = '0';
		buffer[i] = -cline_size;
	}
}

//send data in the start of the cycle
void Cache::Start()
{
	mdata_t item;

//	printf("C: Start next cycle!\n");
//	std::cout << "C: cycle = " << Sim::cycle << "\n";

	//send data to memory
	if(miss && !wait){
		if(!m_in.busy){	//the memory is not busy
			if(dirty){
				printf("C: dirty item = %c %lu\n", dirty_item.type, dirty_item.addr);

				//send the item to the output port
				m_out.data = dirty_item;
				dirty = 0;
				//We don't have to wait in this case
				respond = Sim::cycle + cycles_t(latency);
			}
			else{
				printf("C: miss item = %c %lu\n", miss_item.type, miss_item.addr);

				//send the item to the output port
				m_out.data = miss_item;

				//set the waiting flag
				
				wait = 1;
				wait_addr = miss_item.addr;

				printf("C: wait = %d, wait_addr = %lu\n", wait, wait_addr);
			}
		}

	}
	//send data to the processor
	else if (!miss && Sim::cycle >= respond) {
		item.type = 'W';
		item.addr = respond_addr;
		printf("C(s): sending %lu\n", respond_addr);
		p_out.data = item;
		respond = NEVER;
	}
}

//read data in the end of the cycle
void Cache::End()
{
	mdata_t item;
	int pos;	//mapped position of the item in the cache


	//read from processor
	if (!wait) {	//while I'm waiting response from the memory, I shouldn't process data from the processor
		if (!p_in.data.isNothing()) {
			item = p_in.data;

			printf("C: item = %c %lu\n", item.type, item.addr);

			//check if data is in the cache
			//direct-mapped cache: pos = (addr/clines)mod cline_size
			pos = (int)((item.addr/cline_size)%clines);
				
			//if(buffer[pos] <= item.addr && item.addr < buffer[pos]+cline_size){
			if(buffer[pos] == item.addr/cline_size){
			// HIT //
				printf("C(s): It's a hit! %lu\n", item.addr);
				printf("C: pos = %d, data = %lu, address = %lu\n", pos, buffer[pos], item.addr);
				miss = 0;
				//if READ -> just set respond
				respond = Sim::cycle + cycles_t(latency);
				respond_addr = item.addr;
				//if WRITE -> also set dirty bit
				if (item.type == 'W') {
					dirty_bit_buffer[pos] = '1';
				}
				setNextCycle(respond);
			}
			else{
			// MISS //
				printf("C(s): It's a miss :( %lu\n", item.addr);
				miss = 1;
				miss_item = p_in.data;
				setNextCycle(Sim::cycle + cycles_t(1));
			}
			
			if (respond != NEVER) {
				p_out.busy = true;
			} else {
				p_out.busy = false;
			}
		}
	}
	//read from memory 
	else if (wait && !m_in.data.isNothing()) {
		printf("C: Received data from Memory!\n");
		wait = 0;

		item = m_in.data;
		respond_addr = item.addr;
		//and it's not the data I'm waiting for
		if (item.addr != wait_addr) {
			fprintf(stderr, "C: ERROR: received wrong address\n");
		}
		//check the dirty bit
		pos = (int)((item.addr/clines)%cline_size);

		if(dirty_bit_buffer[pos] == '1'){	
			printf("C: Dirty bit at line position %d!\n", pos);
			//what we received is going to replace a line that has been updated and has to be sent to the memory
			dirty = 1;
			dirty_item.type = 'W';
			dirty_item.addr = buffer[pos];
			dirty_item.size = cline_size;					
			setNextCycle(Sim::cycle + cycles_t(1)); 
			respond = NEVER;	//we don't want to speak to the processor in the next cycle	
		}
		else{
			printf("C: No Dirty bit!\n");			
			miss = 0;	//I just received data from the memory
			respond = Sim::cycle + cycles_t(latency);
			setNextCycle(respond); 	

		}
		//update the cache
		buffer[pos] = (item.addr/clines)*clines;
		//I just received from the memory
		//I don't have to wait for anything
	}

	if (wait || m_in.busy) {
		//printf("C: setting cycle to NEVER\n");
		setNextCycle(NEVER);
	}
}

void Cache::print_hook(void) const {}

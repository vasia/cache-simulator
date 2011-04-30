#include "cache.hpp"
#include <stdio.h>

Cache::Cache(int delay) : Cache("Cache", address_t(0), 0)
{
	latency = delay;

	out.data.setModule(this);
	out.busy.setModule(this);
	in.data.setModule(this);
	in.busy.setModule(this);
}

void Cache::Reset()
{
	respond = NEVER;
	out.data.sendNothing();
	out.busy = false;
	in.busy = false;
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

	if (!in.data.isNothing()) {
		item = in.data;

printf("C: item = %c %lu\n", item.type, item.addr);

		if (item.type == 'R') {
printf("C: is load!\n");
			respond = Sim::cycle + cycles_t(latency);
			respond_addr = item.addr;
std::cout << "C: respond = " << respond << "\n";
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


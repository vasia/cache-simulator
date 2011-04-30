#include "memory.hpp"
#include <stdio.h>

Memory::Memory(int delay) : Module("Memory", address_t(0), 0)
{
	latency = delay;

	out.data.setModule(this);
	out.busy.setModule(this);
	in.data.setModule(this);
}

void Memory::Reset()
{
	respond = NEVER;
	out.data.sendNothing();
	out.busy = false;
}

void Memory::Start()
{
	mdata_t item;

	if (Sim::cycle >= respond) {
		item.type = 'W';
		item.addr = respond_addr;
printf("M(s): sending %lu\n", respond_addr);
		out.data = item;
		respond = NEVER;
	}
}

void Memory::End()
{
	mdata_t item;

	if (!in.data.isNothing()) {
		item = in.data;

printf("M: item = %c %lu\n", item.type, item.addr);

		if (item.type == 'R') {
printf("M: is load!\n");
			respond = Sim::cycle + cycles_t(latency);
			respond_addr = item.addr;
std::cout << "M: respond = " << respond << "\n";
		}
	}

	if (respond != NEVER) {
		out.busy = true;
	} else {
		out.busy = false;
	}

	setNextCycle(respond);
}

void Memory::print_hook(void) const {}


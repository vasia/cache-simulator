#include "bus.hpp"
#include <stdio.h>

Bus::Bus(int id) : Module("Bus", address_t(0), 0)
{
	bid = id;
}

void Bus::Reset()
{
	respond = NEVER;
	l1_out1.data.sendNothing();
	l1_out1.busy = false;
	l1_out2.data.sendNothing();
	l1_out2.busy = false;
	l2_out.data.sendNothing();
	wait = 0;
}

void Bus::Start()
{
	mdata_t item;
	
}

//read data in the end of the cycle
void Bus::End()
{
	mdata_t item;
}

void Bus::print_hook(void) const {
}

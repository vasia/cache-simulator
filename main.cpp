#include "processor.hpp"
#include "memory.hpp"
#include "cache.hpp"

int 
main(int argc, char **argv)
{
	Sim::initSim();
	Sim *sim = Sim::getSim();

	int delay = 1;
	int lines = 100;
	int line_size = 128;	//has to be a multiple of 8
	int cachel1_id1 = 1;
	int cachel1_id2 = 2;
	int cachel2_id = 10;
	int processor_id1 = 1;
	int processor_id2 = 2;

	//allocate buffers
	unsigned long *buffer;
	char *dirty_bit_buffer;
	buffer = new unsigned long[lines];
	dirty_bit_buffer = new char[lines];

	Processor p(processor_id1, "small1.out");
	
	Cache l1(cachel1_id1, delay, lines, line_size, buffer, dirty_bit_buffer);
	//Cache l2();
	//Bus b();
	Memory m(1);

	/*p.out.data >> m.in.data;
	m.out.busy >> p.in.busy;
	m.out.data >> p.in.data;*/

	p.out.data >> l1.p_in.data;
	l1.p_out.busy >> p.in.busy;
	l1.p_out.data >> p.in.data;
	l1.m_out.data >> m.in.data;
	m.out.busy >> l1.m_in.busy;
	m.out.data >> l1.m_in.data;

	

	sim->Reset();
	sim->Run();
	sim->Print();

	delete []buffer;
	delete []dirty_bit_buffer;

	return 0;
}


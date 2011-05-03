#include "processor.hpp"
#include "memory.hpp"
#include "cache.hpp"
#include "bus.hpp"

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

	Processor p1(processor_id1, "small1.out");
	Processor p2(processor_id1, "small1.out");
	
	Cache l1_1(cachel1_id1, delay, lines, line_size, buffer, dirty_bit_buffer);
	Cache l1_2(cachel1_id2, delay, lines, line_size, buffer, dirty_bit_buffer);

	Cache l2(cachel2_id, 5*delay, 5*lines, line_size, buffer, dirty_bit_buffer);

	Bus b(3);

	Memory m(1);

	/*p.out.data >> m.in.data;
	m.out.busy >> p.in.busy;
	m.out.data >> p.in.data;*/

	/*p.out.data >> l1.p_in.data;
	l1.p_out.busy >> p.in.busy;
	l1.p_out.data >> p.in.data;
	l1.m_out.data >> m.in.data;
	m.out.busy >> l1.m_in.busy;
	m.out.data >> l1.m_in.data; */


	//Port connections of p1 and l1_1
	p1.out.data >> l1_1.p_in.data;
	l1_1.p_out.busy >> p1.in.busy;
	l1_1.p_out.data >> p1.in.data;

	//Port connections of p2 and l1_2
	p2.out.data >> l1_2.p_in.data;
	l1_2.p_out.busy >> p2.in.busy;
	l1_2.p_out.data >> p2.in.data;

	//Port connections of l1_1 and bus
	l1_1.m_out.data >> b.l1_in1.data;
	b.l1_out1.busy >> l1_1.m_in.busy;
	b.l1_out1.data >> l1_1.m_in.data;
/*
	//Port connections of l1_2 and bus
	l1_2.m_out.data >> b.l1_in2.data;
	b.l1_out2.busy >> l1_2.m_in.busy;
	b.l1_out2.data >> l1_2.m_in.data;

	//Port connections of bus and l2
	b.l2_out.data >> l2.p_in.data;
	l2.p_out.busy >> b.l2_in.busy;
	l2.p_out.data >> b.l2_in.data;
*/
	//Port connections of l2 and memory
	l2.m_out.data >> m.in.data;
	m.out.busy >> l2.m_in.busy;
	m.out.data >> l2.m_in.data;

	sim->Reset();
	sim->Run();
	sim->Print();

	delete []buffer;
	delete []dirty_bit_buffer;

	return 0;
}


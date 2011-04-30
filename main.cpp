#include "processor.hpp"
#include "memory.hpp"
#include "cache.hpp"

int 
main(int argc, char **argv)
{
	Sim::initSim();
	Sim *sim = Sim::getSim();

	Processor p("small1.out");
	Cache l1(1, 3, 2);
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

	return 0;
}


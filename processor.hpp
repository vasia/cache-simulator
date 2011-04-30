#include <stdio.h>
#include <engine/cyclesim.hpp>
#include "mdata.h"

class Processor : public Module {
public:
	struct {
		Port<mdata_t> data;
		ControlPort busy;
	} in;

	struct {
		Port<mdata_t> data;
	} out;

	Processor(char *file);

    virtual void Reset();
    virtual void Start();
    virtual void End();
    virtual void print_hook(void) const;

private:
	FILE *trace;
	int wait;
	unsigned long wait_addr;
};


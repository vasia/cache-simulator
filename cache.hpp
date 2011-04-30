#include <stdio.h>
#include <engine/cyclesim.hpp>
#include "mdata.h"

class Cache : public Module {
public:
	struct {
		Port<mdata_t> data;
		ControlPort busy;
	} in;

	struct {
		Port<mdata_t> data;
		ControlPort busy;
	} out;

	Cache(int delay, int lines, int line_size);

    virtual void Reset();
    virtual void Start();
    virtual void End();
    virtual void print_hook(void) const;

private:
	unsigned int latency;
	int wait;
	cycles_t respond;
	unsigned long respond_addr;
	unsigned long *buffer;	//it contains only the first address of the cache line
	char *dirty_bit_buff;	//dirty-bit buffer for the write-back policy
	};


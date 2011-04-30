#include <engine/cyclesim.hpp>
#include "mdata.h"

class Memory : public Module {
public:
	struct {
		Port<mdata_t> data;
	} in;

	struct {
		Port<mdata_t> data;
		ControlPort busy;
	} out;

	Memory(int delay);

    virtual void Reset();
    virtual void Start();
    virtual void End();
    virtual void print_hook(void) const;

private:
	unsigned int latency;
	cycles_t respond;
	unsigned long respond_addr;
};


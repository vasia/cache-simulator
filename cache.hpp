#include <stdio.h>
#include <engine/cyclesim.hpp>
#include "mdata.h"

class Cache : public Module {
public:
	struct {
		Port<mdata_t> data;
		//ControlPort busy;
	} p_in;

	struct {
		Port<mdata_t> data;
		ControlPort busy;
	} p_out;

	struct {
		Port<mdata_t> data;
		ControlPort busy;
	} m_in;

	struct {
		Port<mdata_t> data;
		//ControlPort busy;
	} m_out;

	Cache(int delay, int lines, int line_size, unsigned long *cache_buffer, char *cache_dirty_bit_buff);

    virtual void Reset();
    virtual void Start();
    virtual void End();
    virtual void print_hook(void) const;

private:
	unsigned int latency;
	int clines;
	int cline_size;
	int wait;
	unsigned long wait_addr;
	cycles_t respond;
	unsigned long respond_addr;
	unsigned long *buffer;	//it contains only the first address of the cache line
	char *dirty_bit_buffer;	//dirty-bit buffer for the write-back policy
	bool miss;
	mdata_t miss_item;
	bool write_back;
	mdata_t write_back_item;
	int hits, misses;
};


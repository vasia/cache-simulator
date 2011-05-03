#include <stdio.h>
#include <engine/cyclesim.hpp>
#include "mdata.h"

class Bus : public Module {
public:
	struct {
		Port<mdata_t> data;
	} l1_in1;

	struct {
		Port<mdata_t> data;
	} l1_in2;

	struct {
		Port<mdata_t> data;
		ControlPort busy;
	} l2_in;

	struct {
		Port<mdata_t> data;
		ControlPort busy;
	} l1_out1;

	struct {
		Port<mdata_t> data;
		ControlPort busy;
	} l1_out2;

	struct {
		Port<mdata_t> data;
	} l2_out;


	Bus(int id);

    virtual void Reset();
    virtual void Start();
    virtual void End();
    virtual void print_hook(void) const;

private:
	int wait;
	unsigned long wait_addr;
	cycles_t respond;
	unsigned long respond_addr;
	int bid;
};


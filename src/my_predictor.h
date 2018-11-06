// my_predictor.h
// This file contains a sample my_predictor class.
// It is a simple 32,768-entry gshare with a history length of 15.
// Note that this predictor doesn't use the whole 32 kilobytes available
// for the CBP-2 contest; it is just an example.

#include <cstdint>
#define HISTORY_LENGTH	        5
#define BRANCH_INDEX_LENGTH	10

struct table_entry {
    uint8_t history;                        // using HISTORY_LENGTH bits
    uint8_t predictions[1 << HISTORY_LENGTH];   // a prediction for each possible history
};

class correlating_update : public branch_update {
public:
	unsigned int tableIndex;
	unsigned int addressIndex;
};

class correlating_predictor : public branch_predictor {
public:
	correlating_update	u;
	branch_info	bi;
        table_entry tables[1 << BRANCH_INDEX_LENGTH];

	correlating_predictor () {
		memset (tables, 0, sizeof (tables));
	}

	branch_update *predict (branch_info & b) {
	        bi = b;
		if (b.br_flags & BR_CONDITIONAL) {
		    // get table from history and get last 10 bits of branch address
		    u.addressIndex = b.address & 1023;
		    u.tableIndex = tables[u.addressIndex].history;
		    u.direction_prediction (tables[u.addressIndex].predictions[u.tableIndex]);
		} else {
			u.direction_prediction (true);
		}
		u.target_prediction (0);
		return &u;
	}

	void update (correlating_update *u, bool taken, unsigned int target) {
		if (bi.br_flags & BR_CONDITIONAL) {
			// unsigned char	*c = &tables[((correlating_update*)u)->tableIndex][((correlating_update*)u)->addressIndex];
                        uint8_t &prediction = tables[u->addressIndex].predictions[u->tableIndex];
			if (taken) {
			  switch (prediction) {
			    case 0b00:
			      prediction = 0b01;
			      break;
			    case 0b01:
                              prediction = 0b11;
			      break;
			    case 0b10:
                              prediction = 0b01;
			      break;
			    case 0b11:
			      //do nothing
			      break;
			  default: ; //should print error
			  }
			}
			else {
			  switch (prediction) {
			    case 0b00:
			      //do nothing
			      break;
			    case 0b01:
                              prediction = 0b10;
			      break;
			    case 0b10:
                              prediction = 0b00;
			      break;
			    case 0b11:
                              prediction = 0b10;
			      break;
			  default: ; //should print error
			  }

			}
			tables[u->addressIndex].history <<= 1;
			tables[u->addressIndex].history	 |= taken;
			tables[u->addressIndex].history &= (1<<HISTORY_LENGTH)-1;
		}
	}
};

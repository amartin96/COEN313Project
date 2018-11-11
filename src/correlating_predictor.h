// my_predictor.h
// This file contains a sample my_predictor class.
// It is a simple 32,768-entry gshare with a history length of 15.
// Note that this predictor doesn't use the whole 32 kilobytes available
// for the CBP-2 contest; it is just an example.

class correlating_update : public branch_update {
public:
	unsigned int tableIndex;
	unsigned int addressIndex;
};

class correlating_predictor : public branch_predictor {
public:
#define HISTORY_LENGTH	5
#define BRANCH_INDEX_LENGTH	10
        correlating_update	u;
	branch_info	bi;
	unsigned int history;
	unsigned char tables[1<<HISTORY_LENGTH][1<<BRANCH_INDEX_LENGTH];

	correlating_predictor (void) : history(0) {
		memset (tables, 0, sizeof (tables));
	}

	branch_update *predict (branch_info & b) {
		bi = b;
		if (b.br_flags & BR_CONDITIONAL) {

			// get table from history and get last 10 bits of branch address
		  u.tableIndex = history;
			u.addressIndex = b.address & 1023;
		  u.direction_prediction (tables[u.tableIndex][u.addressIndex] >> 1);
		} else {
			u.direction_prediction (true);
		}
		u.target_prediction (0);
		return &u;
	}

	void update (branch_update *u, bool taken, unsigned int target) {
		if (bi.br_flags & BR_CONDITIONAL) {
			unsigned char	*c = &tables[((correlating_update*)u)->tableIndex][((correlating_update*)u)->addressIndex];
			if (taken) {
			  switch (*c) {
			    case 0:
			      *c =1;
			      break;
			    case 1:
			      *c =3;
			      break;
			    case 2:
			      *c=3;
			      break;
			    case 3:
			      //do nothing
			      break;
			  default: ; //should print error
			  }
			}
			else {
			  switch (*c) {
			    case 0:
			      //do nothing
			      break;
			    case 1:
			      *c = 0;
			      break;
			    case 2:
			      *c= 0;
			      break;
			    case 3:
			      *c=2;
			      break;
			  default: ; //should print error
			  }

			}
			history <<= 1;
			history	 |= taken;
			history &= (1<<HISTORY_LENGTH)-1;
		}
	}
};

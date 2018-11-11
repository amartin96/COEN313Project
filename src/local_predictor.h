// my_predictor.h
// This file contains a sample my_predictor class.
// It is a simple 32,768-entry gshare with a history length of 15.
// Note that this predictor doesn't use the whole 32 kilobytes available
// for the CBP-2 contest; it is just an example.

class local_update : public branch_update {
public:
	unsigned int	index;
};

class local_predictor : public branch_predictor {
public:
	#define TABLE_BITS 15
	local_update	u;
	branch_info	bi;
	unsigned int	history;
	unsigned char	tab[1<<TABLE_BITS];

	local_predictor (void) : history(0) {
		memset (tab, 0, sizeof (tab));
	}

	branch_update *predict (branch_info & b) {
		bi = b;
		if (b.br_flags & BR_CONDITIONAL) {
		  u.index = b.address%(1<<TABLE_BITS);

		  u.direction_prediction (tab[u.index] >> 1);
		} else {
			u.direction_prediction (true);
		}
		u.target_prediction (0);
		return &u;
	}

	void update (branch_update *u, bool taken, unsigned int target) {
		if (bi.br_flags & BR_CONDITIONAL) {
			unsigned char	*c = &tab[((local_update*)u)->index];
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
		}
	}
};

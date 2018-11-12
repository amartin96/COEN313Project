// my_predictor.h
// This file contains a sample my_predictor class.
// It is a simple 32,768-entry gshare with a history length of 15.
// Note that this predictor doesn't use the whole 32 kilobytes available
// for the CBP-2 contest; it is just an example.

#include "local_predictor.h"
#include "my_predictor.h"

class tournament_update : public branch_update {
public:
	// 0 is local, 1 is gshare
	unsigned int chosenPredictor;
	// update from local predictor
	local_update* localUpdate;
	// update from gshare my_predictor
	my_update* gshareUpdate;

};

class tournament_predictor : public branch_predictor {
public:
// #define HISTORY_LENGTH 128
	tournament_update	u;
	branch_info	bi;

	// local and gshare predictor objects
	branch_predictor *local;
	branch_predictor *gshare;

	// current predictions for local and gshare
	bool currentLocalPrediction;
	bool currentGsharePrediction;

	/*current state
		strong local: 3
		weak local: 2
		weak gshare: 1
		strong gshare: 0
	*/
	int currentState;

	tournament_predictor (void) {
		currentState = 2;
		local = new local_predictor();
		gshare = new my_predictor();
	}

	branch_update *predict (branch_info &b) {
		bi = b;
		// get predictions
		u.localUpdate = (local_update*) local->predict(b);
		currentLocalPrediction = (u.localUpdate)->direction_prediction();
		u.gshareUpdate = (my_update*) gshare->predict(b);
		currentGsharePrediction = (u.gshareUpdate)->direction_prediction();
		if (b.br_flags & BR_CONDITIONAL) {
		  if(currentState >= 2) {
				// use local predictor
				u.chosenPredictor = 0;
				// make the guess
				u.direction_prediction (currentLocalPrediction);
			} else {
				// use gshare
				u.chosenPredictor = 1;
				// make the guess
				u.direction_prediction (currentGsharePrediction);
			}
		} else {
			u.direction_prediction (true);
		}
		u.target_prediction (0);
		return &u;
	}

	void update (branch_update *u, bool taken, unsigned int target) {
			// int highestLocalPredictorBit = localSuccess & (1 << ((sizeof(highestLocalPredictorBit) * 8) - 1));
			// int highestGsharePredictorBit = gshareSuccess & (1 << ((sizeof(highestGsharePredictorBit) * 8) - 1));
			tournament_update* update = (tournament_update*) u;
			local->update(update->localUpdate,taken,target);
			gshare->update(update->gshareUpdate,taken,target);
			// branch was predicted correctly
			if(update->chosenPredictor == 0) {
				if(currentState == 3) {
					if(taken) {
						// it remains strong local (1,0), (1,1)
					} else {
						if(currentLocalPrediction == currentGsharePrediction) {
							// it remains weak local (0,0)
						} else {
							// changes to weak local (0,1)
							currentState = 2;
						}
					}
				} else if(currentState == 2) {
					if(taken) {
						if(currentLocalPrediction != currentGsharePrediction) {
							// changes to strong local (1,0)
							currentState = 3;
						} else {
							// it remains the same (1,1)
						}
					} else {
						if(currentLocalPrediction == currentGsharePrediction) {
							// it remains the same (0,0)
						} else {
							// changes to strong gshare (0,1)
							currentState = 0;
						}
					}
				} else if(currentState == 1) {
					if(taken) {
						if(currentLocalPrediction == currentGsharePrediction) {
							// remains the same (1,1)
						} else {
							// changes to strong local (1,0)
							currentState = 3;
						}
					} else {
						if(currentLocalPrediction == currentGsharePrediction) {
							// remains the same (0,0)
						} else {
							// changes to strong gshare (0,1)
							currentState = 0;
						}
					}
				} else if(currentState == 0) {
					if(taken) {
						if(currentLocalPrediction != currentGsharePrediction) {
							// changes to weak gshare (1,0)
							currentState = 1;
						} else {
							// remains the same (1,1)
						}
					} else {
						if(currentLocalPrediction == currentGsharePrediction) {
							// remains the same (0,0)
						} else {
							// remains the same (0,1)
						}
					}
				}
			} else if(update->chosenPredictor == 1) {
				if(currentState == 0) {
					if(taken) {
						// it remains strong gshare (0,1), (1,1)
					} else {
						if(currentLocalPrediction == currentGsharePrediction) {
							// it remains weak local (0,0)
						} else {
							// changes to weak gshare (1,0)
							currentState = 1;
						}
					}
				} else if(currentState == 1) {
					if(taken) {
						if(currentLocalPrediction != currentGsharePrediction) {
							// changes to strong local (0,1)
							currentState = 0;
						} else {
							// it remains the same (1,1)
						}
					} else {
						if(currentLocalPrediction == currentGsharePrediction) {
							// it remains the same (0,0)
						} else {
							// changes to strong gshare (1,0)
							currentState = 3;
						}
					}
				} else if(currentState == 2) {
					if(taken) {
						if(currentLocalPrediction == currentGsharePrediction) {
							// remains the same (1,1)
						} else {
							// changes to strong local (0,1)
							currentState = 0;
						}
					} else {
						if(currentLocalPrediction == currentGsharePrediction) {
							// remains the same (0,0)
						} else {
							// changes to strong gshare (1,0)
							currentState = 3;
						}
					}
			} else if(currentState == 3) {
				if(taken) {
					if(currentLocalPrediction == currentGsharePrediction) {
						// remains the same (1,1)
					} else {
						// changes to weak gshare (0,1)
						currentState = 2;
					}
				} else {
					if(currentLocalPrediction == currentGsharePrediction) {
						// remains the same (0,0)
					} else {
						// remains the same (1,0)
					}
				}
			}
		}
	}
};

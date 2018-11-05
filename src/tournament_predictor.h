// my_predictor.h
// This file contains a sample my_predictor class.
// It is a simple 32,768-entry gshare with a history length of 15.
// Note that this predictor doesn't use the whole 32 kilobytes available
// for the CBP-2 contest; it is just an example.

// #include "local_predictor.h"
// #include "my_predictor.h"

class tournament_update : public branch_update {
public:
	// 0 is local, 1 is gshare
	unsigned int chosenPredictor;
};

class tournament_predictor : public branch_predictor {
public:
// #define HISTORY_LENGTH 128
	tournament_update	u;
	branch_info	bi;

	// bit shift histories of the success of local and gshare predictors
	unsigned long	localHistory;
	unsigned long gshareHistory;

	// current counts of successes of local and gshare predictors
	unsigned int localSuccess;
	unsigned int gshareSuccess;

	// local and gshare predictor objects
	branch_predictor *local;
	branch_predictor *gshare;

	// current predictions for local and gshare
	bool currentLocalPrediction;
	bool currentGsharePrediction;


	tournamnet_predictor (void) : history(0) {
		localHistory = 0;
		gshareHistory = 0;
		localSuccess = 0;
		gshareSuccess = 0;
		local = new local_predictor();
		gshare = new my_predictor();
	}

	branch_update *predict (branch_info &b) {
		bi = b;
		// get predictions
		currentLocalPrediction = local->predict(b)).direction_prediction();
		currentGsharePrediction = gshare->predict(b)).direction_prediction();
		if (b.br_flags & BR_CONDITIONAL) {
		  if(localHistory >= gshareHistory) {
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
		int highestLocalPredictorBit = localSuccess & (1 << ((sizeof(highestLocalPredictorBit) * 8) - 1));
		int highestGsharePredictorBit = gshareSuccess & (1 << ((sizeof(highestGsharePredictorBit) * 8) - 1));
		tournament_update* update = (tournament_update*) u;
		// branch was predicted correctly
		if(update->taken) {
			// update local predictor if local prediction was 1 (correct)
			if(currentLocalPrediction) {
				// local predictor history shift register has highest bit of 1
				if(highestLocalPredictorBit) {
					// shift 1 to the left and add one on the right
					localHistory <<= 1;
					localHistory += 1;
				} else {
					// shift 1 to the left and add one on the right, as well as add additional success
					localHistory <<= 1;
					localHistory += 1;
					localSuccess += 1;
				}
			} else {
				// local predictor history shift register has highest bit of 1
				if(highestLocalPredictorBit) {
					// shift 1 to the left, leave 0 on the right, and subtract 1 success
					localHistory <<= 1;
					localSuccess -= 1;
				} else {
					// shift 1 to the left, leave 0 on the right, and don't change success
					localHistory <<= 1;
				}
			}

			// update gshare predictor if gshare prediction was 1 (correct)
			if(currentGsharePrediction) {
				// local predictor history shift register has highest bit of 1
				if(highestGsharePredictorBit) {
					// shift 1 to the left and add one on the right
					gshareHistory <<= 1;
					gshareHistory += 1;
				} else {
					// shift 1 to the left and add one on the right, as well as add additional success
					gshareHistory <<= 1;
					gshareHistory += 1;
					gshareSuccess += 1;
				}
			} else {
				// local predictor history shift register has highest bit of 1
				if(highestGsharePredictorBit) {
					// shift 1 to the left, leave 0 on the right, and subtract 1 success
					gshareHistory <<= 1;
					gshareSuccess -= 1;
				} else {
					// shift 1 to the left, leave 0 on the right, and don't change success
					gshareHistory <<= 1;
				}
			}
		} else {
			// update local predictor if local prediction was 1 (incorrect)
			if(currentLocalPrediction) {
				// local predictor history shift register has highest bit of 1
				if(highestLocalPredictorBit) {
					// shift 1 to the left, leave 0 on the right, subtract 1 success
					localHistory <<= 1;
					localHistory -= 1;
				} else {
					// shift 1 to the left, leave 0 on the right
					localHistory <<= 1;
				}
			} else {
				// local predictor history shift register has highest bit of 1
				if(highestLocalPredictorBit) {
					// shift 1 to the left, add 1 on the right, leave successes
					localHistory <<= 1;
					localHistory += 1;
				} else {
					// shift 1 to the left, add 1 on the right, add 1 to successes
					localHistory <<= 1;
					localHistory += 1;
					localSuccess += 1;
				}
			}

			// update gshare predictor if local prediction was 1 (incorrect)
			if(currentGsharePrediction) {
				// local predictor history shift register has highest bit of 1
				if(highestGsharePredictorBit) {
					// shift 1 to the left, leave 0 on the right, subtract 1 success
					gshareHistory <<= 1;
					gshareSuccess -= 1;
				} else {
					// shift 1 to the left, leave 0 on the right
					gshareHistory <<= 1;
				}
			} else {
				// local predictor history shift register has highest bit of 1
				if(highestGsharePredictorBit) {
					// shift 1 to the left, add 1 on the right, leave successes
					gshareHistory <<= 1;
					gshareHistory += 1;
				} else {
					// shift 1 to the left, add 1 on the right, add 1 to successes
					gshareHistory <<= 1;
					gshareHistory += 1;
					gshareSuccess += 1;
				}
			}
		}
	}
};

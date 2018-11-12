// my_predictor.h
// This file contains a sample my_predictor class.
// It is a simple 32,768-entry gshare with a history length of 15.
// Note that this predictor doesn't use the whole 32 kilobytes available
// for the CBP-2 contest; it is just an example.

#include "local_predictor.h"
#include "my_predictor.h"

enum PredictorType {
    LOCAL,
    GSHARE
};

enum TournamentState {
    STRONG_LOCAL,
    WEAK_LOCAL,
    WEAK_GSHARE,
    STRONG_GSHARE
};

class tournament_update : public branch_update {
public:
    PredictorType chosenPredictor;
    local_update* localUpdate;
    my_update* gshareUpdate;
};

class tournament_predictor : public branch_predictor {
public:
    tournament_update u;
    branch_info bi;

    // local and gshare predictor objects
    local_predictor local;
    my_predictor gshare;

    // current predictions for local and gshare
    bool currentLocalPrediction;
    bool currentGsharePrediction;

    TournamentState currentState;

    tournament_predictor (void) {
        currentState = WEAK_GSHARE;
    }

    branch_update *predict (branch_info &b) {
        bi = b;

        if (b.br_flags & BR_CONDITIONAL) {
            // get predictions
            u.localUpdate = (local_update*) local.predict(b);
            currentLocalPrediction = u.localUpdate->direction_prediction();
            u.gshareUpdate = (my_update*) gshare.predict(b);
            currentGsharePrediction = u.gshareUpdate->direction_prediction();

            if(currentState == STRONG_LOCAL || currentState == WEAK_LOCAL) {
                // use local predictor
                u.chosenPredictor = LOCAL;
                // make the guess
                u.direction_prediction (currentLocalPrediction);
            } else {
                // use gshare
                u.chosenPredictor = GSHARE;
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
            tournament_update* update = (tournament_update*) u;
            local.update(update->localUpdate, taken, target);
            gshare.update(update->gshareUpdate, taken, target);

            bool local_correct = currentLocalPrediction == taken;
            bool gshare_correct = currentGsharePrediction == taken;

            switch (currentState) {

                case STRONG_LOCAL:
                    if ((!local_correct && !gshare_correct) || local_correct) /* do nothing */;    // 00, 10, 11
                    else currentState = WEAK_LOCAL;                                              // 01
                    break;

                case WEAK_LOCAL:
                    if (local_correct == gshare_correct) /* do nothing */;  // 00, 11
                    else if (local_correct) currentState = STRONG_LOCAL;    // 10
                    else currentState = STRONG_GSHARE;                      // 01
                    break;

                case WEAK_GSHARE:
                    if (local_correct == gshare_correct) /* do nothing */;  // 00, 11
                    else if (gshare_correct) currentState = STRONG_GSHARE;  // 01
                    else currentState = STRONG_LOCAL;                       // 10
                    break;

                case STRONG_GSHARE:
                    if ((!local_correct && !gshare_correct) || gshare_correct) /* do nothing */;  // 00, 01, 11
                    else currentState = WEAK_GSHARE;                                            // 10
                    break;
            }
    }
};

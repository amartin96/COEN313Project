// my_predictor.h
// This file contains a sample my_predictor class.
// It is a simple 32,768-entry gshare with a history length of 15.
// Note that this predictor doesn't use the whole 32 kilobytes available
// for the CBP-2 contest; it is just an example.

#include <cstdint>
#define CUSTOM_HISTORY_LENGTH   5
#define BRANCH_INDEX_LENGTH     10

struct table_entry {
    uint8_t history;                                   // using CUSTOM_HISTORY_LENGTH bits
    uint8_t predictions[1 << CUSTOM_HISTORY_LENGTH];   // a prediction for each possible history
};

class custom_update : public branch_update {
public:
    unsigned int tableIndex;
    unsigned int addressIndex;
};

class custom_predictor : public branch_predictor {
public:
    custom_update u;
    branch_info    bi;
    table_entry tables[1 << BRANCH_INDEX_LENGTH];

    custom_predictor () {
        memset (tables, 0, sizeof (tables));
    }

    branch_update *predict (branch_info & b) {
        bi = b;
        if (b.br_flags & BR_CONDITIONAL) {
            // get table from history and get last 10 bits of branch address
            u.addressIndex = b.address & 1023;
            u.tableIndex = tables[u.addressIndex].history;

            // get the prediction
            bool prediction;
            switch(tables[u.addressIndex].predictions[u.tableIndex]) {
            case 0b00:
            case 0b01:
                prediction = false;
                break;
            default:
                prediction = true;
            }
            u.direction_prediction (prediction);
        } else {
            u.direction_prediction (true);
        }
        u.target_prediction (0);
        return &u;
    }

    /* 00: strongly not taken
     * 01: weakly not taken
     * 10: weakly taken
     * 11: strongly taken
     */
    void update (branch_update *not_u, bool taken, unsigned int target) override {
        if (bi.br_flags & BR_CONDITIONAL) {
            auto u = (custom_update*)not_u;
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
            tables[u->addressIndex].history |= taken;
            tables[u->addressIndex].history &= (1<<CUSTOM_HISTORY_LENGTH)-1;
        }
    }
};

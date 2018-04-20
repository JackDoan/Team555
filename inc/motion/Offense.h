//
// Created by jad140230 on 3/24/2018.
//

#ifndef TEAM555_ATTACK_H
#define TEAM555_ATTACK_H


#include "../states.h"
#include "../../src/GameState.h"

class Offense {
private:
    offenseState_t state = OFFENSEDONE;



private:
    typedef enum offenseDecision_e {STAGE, STRIKE} offenseDecision_t;
    offenseDecision_t offenseDecision = STAGE;
    hitVector_t hitVector;



public:
    Offense() = default;
    void reset() { setDone(); }
    bool run(GameState&);
    offenseState_t getState() const;
    void setDone();

};

#endif //TEAM555_ATTACK_H

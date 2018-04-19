//
// Created by jad140230 on 3/24/2018.
//

#ifndef TEAM555_ATTACK_H
#define TEAM555_ATTACK_H


#include "../states.h"
#include "../GamePieces/Mallet.h"
#include "../GamePieces/Puck.h"

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
    bool run(Mallet& mallet, Puck& puck, cv::Mat& grabbed);
    offenseState_t getState() const;
    void setDone();

};

#endif //TEAM555_ATTACK_H

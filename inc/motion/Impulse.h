//
// Created by mike on 4/19/18.
//

#ifndef TEAM555_IMPULSE_H
#define TEAM555_IMPULSE_H


#include <opencv2/core/mat.hpp>
#include "../states.h"
#include "../../src/GameState.h"

class Impulse {
private:
    std::vector<cv::Point_<int>> impulseVector;
public:

    bool run(GameState& gs);
    impulseState_t impulseState = IMPULSEDONE;
    std::vector<cv::Point_<int>> findImpulseVector(GameState& gs);
};


#endif //TEAM555_IMPULSE_H

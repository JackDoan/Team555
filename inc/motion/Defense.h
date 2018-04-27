//
// Created by mike on 4/19/18.
//

#ifndef TEAM555_DEFENSE_H
#define TEAM555_DEFENSE_H


#include <opencv2/core/types.hpp>

#include "../states.h"
#include "../GameState.h"

class Defense {

private:
    typedef enum defenseDecision_e {GOHOME, INTERCEPT} defenseDecision_t;
    defenseState_t state = ATHOME;
    bool checkPastgoalFlags(std::vector<bool> goalFlagHistory);
    cv::Scalar defenseColor = cv::Scalar(225, 255, 255);


public:
    defenseState_t getState() const;
    Defense() = default;
    cv::Point_<int> run(GameState& gs);

};


#endif //TEAM555_DEFENSE_H

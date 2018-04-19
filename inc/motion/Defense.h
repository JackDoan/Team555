//
// Created by mike on 4/19/18.
//

#ifndef TEAM555_DEFENSE_H
#define TEAM555_DEFENSE_H


#include <opencv2/core/types.hpp>
#include "../Mallet.h"
#include "../Puck.h"
#include "../states.h"

class Defense {

private:
    typedef enum defenseDecision_e {GOHOME, INTERCEPT} defenseDecision_t;
    defenseState_t state = ATHOME;
    bool checkPastgoalFlags(std::vector<bool> goalFlagHistory);


public:
    defenseState_t getState() const;
    Defense() = default;
    cv::Point_<int> run(Mallet& mallet, Puck& puck, cv::Mat& grabbed);
};


#endif //TEAM555_DEFENSE_H

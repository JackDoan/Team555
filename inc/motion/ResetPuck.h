//
// Created by mike on 4/25/18.
//

#ifndef TEAM555_RESETPUCK_H
#define TEAM555_RESETPUCK_H
#include "../states.h"
#include "../GameState.h"
#include <vector>

class ResetPuck {
private:

    typedef enum resetState_e {RESETDONE, FINDINGPUCK, NOPUCK, STAGING1, STAGING2, HITTING} resetState_t;
public:
    resetState_t resetState;
    ResetPuck() {
        resetState = RESETDONE;
    }
    bool run(GameState&, int lostCnt);
    std::vector<cv::Point_<int>> calcStage(GameState&);
    std::vector<cv::Point_<int>> hitVector;
    cv::Point_<int> findingSpot = {Table::motionLimitMin.x, Table::home.y};

};


#endif //TEAM555_RESETPUCK_H

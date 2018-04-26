//
// Created by jad140230 on 3/24/2018.
//

#ifndef TEAM555_ATTACK_H
#define TEAM555_ATTACK_H


#include "../states.h"
#include "../GameState.h"
#include "../../src/MovingAverage.h"

class Offense {
private:
    offenseState_t state = TRACKING;
    cv::Point_<MovingAverage> commands;



public:
    Offense() {
        std::vector<double> weights = {0.3, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1};
        //std::vector<double> weights = {0.2, 0.2, 0.2, 0.2, 0.2};
        //unsigned long weights = 5;
        commands.x = MovingAverage(weights);
        commands.y = MovingAverage(weights);
    }
    cv::Point_<int> run(GameState&, double frameRate);
    offenseState_t getState() const;
    void setDone();

};

#endif //TEAM555_ATTACK_H

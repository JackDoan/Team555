//
// Created by mike on 4/19/18.
//

#ifndef TEAM555_IMPULSE_H
#define TEAM555_IMPULSE_H


#include <opencv2/core/mat.hpp>
#include "../Mallet.h"
#include "../states.h"
#include "../Puck.h"

class Impulse {
private:
    std::vector<cv::Point_<int>> impulseVector;
public:

    bool run(Mallet& mallet, Puck& puck, cv::Mat& grabbed);
    impulseState_t impulseState = IMPULSEDONE;
    std::vector<cv::Point_<int>> findImpulseVector(Mallet& mallet, Puck& puck, cv::Mat& grabbed);
};


#endif //TEAM555_IMPULSE_H

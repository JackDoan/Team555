//
// Created by jad140230 on 3/24/2018.
//

#ifndef TEAM555_ATTACK_H
#define TEAM555_ATTACK_H


#include "Strategy.h"

class Attack : public Strategy{
private:
    bool checkLimits(const Puck& puck, const Mallet& mallet);
    cv::Point_<int> shotDistance = {200, 0};
    cv::Point_<int> overshotDistance = {100, 0};
public:
    Attack(const Table& table, const MotorDriver& motorDriver) {
        this->table = table;
    }
    void run(const Puck& puck, const Mallet& mallet, cv::Mat& grabbed);
};


#endif //TEAM555_ATTACK_H

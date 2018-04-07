//
// Created by jad140230 on 3/24/2018.
//

#ifndef TEAM555_STRATEGY_H
#define TEAM555_STRATEGY_H

#include "../Table.h"
#include "../MotorDriver.h"
#include "../Mallet.h"
#include "../Puck.h"

class Strategy {
protected:
    Table table{};
    bool ready = false;
public:
    Strategy() = default;
    Strategy(const Table& table) {
        this->table = table;
    }
    virtual void run(const Puck& puck, const Mallet& mallet, cv::Mat& grabbed) {};
    bool isReady() { return ready; }
    void setReady(bool b) {ready = b;}
};


#endif //TEAM555_STRATEGY_H

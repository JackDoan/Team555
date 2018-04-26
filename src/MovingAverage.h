//
// Created by mike on 4/22/18.
//

#ifndef TEAM555_MOVINGAVERAGE_H
#define TEAM555_MOVINGAVERAGE_H

#include <cstdlib>
#include <vector>

class MovingAverage {
private:
    std::vector<int> data;
    std::vector<double> weights;
    unsigned long size;

    
public:
    MovingAverage();
    MovingAverage(const unsigned long& size);
    MovingAverage(const std::vector<double>& weights);
    int filter(const int& in);
    void clear();
    
};


#endif //TEAM555_MOVINGAVERAGE_H

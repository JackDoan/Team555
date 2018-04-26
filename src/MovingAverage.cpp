//
// Created by mike on 4/22/18.
//

#include "MovingAverage.h"

MovingAverage::MovingAverage() {
    size = 1;
    weights = {1};
    data = {0};
}

MovingAverage::MovingAverage(const unsigned long& size) {
    this->size = size;
    for(int i = 0; i < size; i++) {
        weights.emplace_back(1/size);
        data.emplace_back(0);
    }
}

MovingAverage::MovingAverage(const std::vector<double>& weights) {
    this->size = weights.size();
    this->weights = weights;
    for(auto item : weights) {
        data.emplace_back(item);
    }
}

int MovingAverage::filter(const int &in) {
    //data.erase(data.end());
    data.insert(data.begin(),in);
    data.resize(size);
    int result = 0;
    for(int i = 0; i < size; i++) {
        result += data[i] * weights[i];
    }
    return static_cast<int>(result);

}

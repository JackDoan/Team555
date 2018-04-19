//
// Created by mike on 4/19/18.
//

#ifndef TEAM555_HISTORY_H
#define TEAM555_HISTORY_H


#include <vector>
#include <opencv2/core/mat.hpp>

template <class T>
class History {
private:

public:
    std::vector<T> data;
    int dataLen = 30;
    cv::Scalar color;
    int diameter;
    explicit History(cv::Scalar mColor = cv::Scalar(50,255,200), int mDiameter=3) {
        color = mColor;
        diameter = mDiameter;
        data = std::vector<T>();
    }

    void insert(const T& d);
};



#endif //TEAM555_HISTORY_H

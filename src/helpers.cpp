//
// Created by mdl150330 on 3/23/2018.
//

#include <opencv2/core/types.hpp>
#include "../inc/helpers.h"

int saturate(int in, int min, int max) {
    int out = in;
    if (in > max) {
        out = max;
    } else if (in < min) {
        out = min;
    }
    return out;
}

cv::Point_<int> saturate(cv::Point_<int> in, cv::Point_<int> min, cv::Point_<int> max) {
    cv::Point_<int> out = {saturate(in.x, min.x, max.x), saturate(in.y, min.y, max.y)};
    return out;
}

bool within(cv::Point_<int> in, cv::Point_<int> min, cv::Point_<int> max) {
    return(saturate(in, min, max) == in);
}
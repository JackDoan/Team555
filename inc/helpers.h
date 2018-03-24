//
// Created by jad140230 on 2/2/2018.
//

#ifndef AIRHOCKEYROBOT_HELPERS_H
#define AIRHOCKEYROBOT_HELPERS_H

#include <utility>

int saturate(int in, int min, int max);

cv::Point_<int> saturate(cv::Point_<int> in, cv::Point_<int> min, cv::Point_<int> max);

bool within(cv::Point_<int> in, cv::Point_<int> min, cv::Point_<int> max);

#define dbgPrint(fmt, ...) \
            do { printf(fmt, ##__VA_ARGS__); } while (0)

#endif //AIRHOCKEYROBOT_HELPERS_H

//
// Created by mike on 4/17/18.
//

#ifndef TEAM555_STATES_H
#define TEAM555_STATES_H
#include <opencv2/core/types.hpp>

typedef enum defenseState_e {ATHOME, GOINGHOME, INTERCEPTING, ATINTERCEPT} defenseState_t;
typedef enum offenseState_e {OFFENSEDONE, GOINGTOSTAGE, STRIKING} offenseState_t;
typedef enum impulseState_e {IMPULSEDONE, STAGING1, STAGING2, HITTING} impulseState_t;

typedef struct hitVector_s {
    std::vector<cv::Point_<int>> data;
    cv::Point_<int> puckplus;
    cv::Point_<int> puckminus;
} hitVector_t;

#endif //TEAM555_STATES_H

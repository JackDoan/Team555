//
// Created by mike on 4/20/18.
//

#ifndef TEAM555_TRAJECTORY_H
#define TEAM555_TRAJECTORY_H


class Trajectory {
private:
    static const int vectorMult;
public:
    static std::vector<std::vector<cv::Point_<int>>> calculate(const cv::Point_<int>& lastLoc, const cv::Point_<int>& loc);
};


#endif //TEAM555_TRAJECTORY_H

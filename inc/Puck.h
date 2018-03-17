//
// Created by jad140230 on 1/21/2018.
//

#ifndef AIRHOCKEYROBOT_PUCK_H
#define AIRHOCKEYROBOT_PUCK_H
#include <opencv2/opencv.hpp>
#include "Table.h"
#include "Thing.h"
#include <vector>
#include <math.h>
#include <thread>


class Puck: public Thing {
private:

public:
    //double lastX = -1;
    // lastY = -1;
    //double x = 0;
    //double y = 0;
    //cv::Point_<int> location, lastLocation, vectorXY, predictedLocation;
//    Coordinate CoordsDouble;
    //int vectorMult = 10;

    int walls[4][3];
    Puck();
    void setGoals(cv::Mat previewSmall, std::vector<cv::Point_<int>> sortedX);
    //void calcVector(cv::Mat in);
    //void drawVector(cv::Mat in);
    //Vector getVector(cv::Mat in);
    //void getCoords(Table table);
    //Coordinate getCoords(Table table);
    //void calcTraj(Table table);
    void setWalls(std::vector<cv::Point_<int>> sortedX, std::vector<cv::Point_<int>> sortedY);

};


#endif //AIRHOCKEYROBOT_PUCK_H

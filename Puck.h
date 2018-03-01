//
// Created by jad140230 on 1/21/2018.
//

#ifndef AIRHOCKEYROBOT_PUCK_H
#define AIRHOCKEYROBOT_PUCK_H
#include <opencv2/opencv.hpp>
#include "ThresholdImage.h"
#include "Table.h"
#include <vector>
#include <math.h>

class Puck {
private:
    int minArea = 1200;
    int maxArea = 3600;
    int minRoundness = 445;
    ThresholdImage thresholdImage = ThresholdImage(threshold_s());

public:
    //double lastX = -1;
    // lastY = -1;
    //double x = 0;
    //double y = 0;
    Coordinate location, lastLocation;
    Coordinate CoordsDouble;

//    double coordX;
//    double coordY;
//    double lastCoordX;
//    double lastCoordY;

    //int puckCoordX;
    //int puckCoordY;
    //int oldCoordX;
    //int oldCoordY;
    double speedX;
    double speedY;
    float speed;         // mm/sec
    float direction;     // radians

    double vectorX;
    double vectorY;


    Puck();
    ~Puck();
    void setupTrackbars();
    double getMinArea();
    double getMaxArea();
    double getMinRoundness();
    static std::vector<cv::Point_<int>> findPucks(cv::Mat in, Table table);    //Changed from int to pointer, returns coords
    cv::Point_<int> find(cv::Mat in, Table table);    //Changed from int to pointer, returns coords

    Vector getVector(cv::Mat in, cv::Point_<int> location, cv::Point_<int> lastLocation);
    //Vector getVector(cv::Mat in);
    //void getCoords(Table table);
    //Coordinate getCoords(Table table);



};


#endif //AIRHOCKEYROBOT_PUCK_H

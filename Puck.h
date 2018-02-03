//
// Created by jad140230 on 1/21/2018.
//

#ifndef AIRHOCKEYROBOT_PUCK_H
#define AIRHOCKEYROBOT_PUCK_H
#include <opencv2/opencv.hpp>
#include "ThresholdImage.h"
#include "Table.h"

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

    //int puckCoordX;
    //int puckCoordY;
    //int oldCoordX;
    //int oldCoordY;
    double speedX;
    double speedY;
    float speed;         // mm/sec
    float direction;     // radians
    int coordX;
    int coordY;
    int lastCoordX;
    int lastCoordY;
    int vectorX;
    int vectorY;


    Puck();
    ~Puck();
    void setupTrackbars();
    double getMinArea();
    double getMaxArea();
    double getMinRoundness();
    void find(cv::Mat in, Table table);
    void getVector(cv::Mat in);
    void getCoords(Table table);


};


#endif //AIRHOCKEYROBOT_PUCK_H

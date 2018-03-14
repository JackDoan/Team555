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
    int minAreaCalib = 800;
    int maxAreaCalib = 4000;
    int minRoundnessCalib = 320;
    ThresholdImage thresholdImage = ThresholdImage(threshold_s());
    ThresholdImage thresholdImageCalib = ThresholdImage(threshold_s());


public:
    //double lastX = -1;
    // lastY = -1;
    //double x = 0;
    //double y = 0;
    cv::Point_<int> location, lastLocation, vectorXY, predictedLocation;
    Coordinate CoordsDouble;
    int vectorMult = 10;


//    double coordX;
//    double coordY;
//    double lastCoordX;
//    double lastCoordY;

    //int puckCoordX;
    //int puckCoordY;
    //int oldCoordX;
    //int oldCoordY;
    int walls[4][3];
    int predicted[3];
    double det;
    cv::Point_<double> intersect;
    double speedX;
    double speedY;
    float speed;         // mm/sec
    float direction;     // radians
    bool bouncex;
    bool bouncey;
    bool puckFound;
    int lostCnt;

//    double vectorX;
//    double vectorY;



    Puck(Table table);
    ~Puck();
    void setupTrackbars();
    double getMinArea();
    double getMaxArea();
    double getMinRoundness();
    std::vector<cv::Point_<int>> findPucks(cv::Mat in, Table table);    //Changed from int to pointer, returns coords
    void findPuck(cv::Mat in, Table table);    //Changed from int to pointer, returns coords

    void calcVector(cv::Mat in);
    void drawVector(cv::Mat in);
    //Vector getVector(cv::Mat in);
    //void getCoords(Table table);
    //Coordinate getCoords(Table table);
    void calcTraj(Table table);
    void setWalls(std::vector<cv::Point_<int>> sortedX, std::vector<cv::Point_<int>> sortedY);

};


#endif //AIRHOCKEYROBOT_PUCK_H

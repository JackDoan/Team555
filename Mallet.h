//
// Created by mdl150330 on 3/14/2018.
//

#ifndef TEAM555_MALLET_H
#define TEAM555_MALLET_H
#include <opencv2/opencv.hpp>
#include "ThresholdImage.h"
#include "Table.h"
#include <vector>
#include <math.h>
#include <thread>



class Mallet {
private:
    int minArea = 1750;
    int maxArea = 4500;
    int minRoundness = 1800;
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

    //int malletCoordX;
    //int malletCoordY;
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
    bool malletFound;
    int lostCnt;

    Mallet(Table table);
    ~Mallet();
    void setupTrackbars();
    double getMinArea();
    double getMaxArea();
    double getMinRoundness();
    void findMallet(cv::Mat in, Table table);    //Changed from int to pointer, returns coords
};


#endif //TEAM555_MALLET_H

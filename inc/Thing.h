//
// Created by jad140230 on 3/17/2018.
//

#ifndef TEAM555_THING_H
#define TEAM555_THING_H


#include <opencv2/opencv.hpp>
#include "Table.h"
#include <vector>
#include <math.h>
#include <thread>
#include <string>

struct threshold_s {
    int minH = 26;
    int maxH = 128;
    int minS = 69;
    int maxS = 138;
    int minV = 59;
    int maxV = 172;

//    int minH = 16;
//    int maxH = 121;
//    int minS = 23;
//    int maxS = 154;
//    int minV = 0;
//    int maxV = 0;
};

class Thing {
protected:
    int minArea = 1200;
    int maxArea = 3600;
    int minRoundness = 445;
    int minAreaCalib = 800;
    int maxAreaCalib = 4000;
    int minRoundnessCalib = 320;
    char* settingWindowName = const_cast<char *>("Settings");
    char* previewWindowName = const_cast<char *>("Thing");
    bool debug = false;
    bool debugWindows = false;
    struct threshold_s limits;
    cv::Mat getThresholdImage(cv::Mat& in);
    cv::Scalar outlineColor = cv::Scalar(0, 255, 0);
    static void onChange(int v, void *ptr) {
        // resolve 'this':
        auto *that = (Thing*)ptr;
        that->realTrack(v);
    }
    void realTrack(int v) {
        limits.minH = cv::getTrackbarPos(TrackbarName[0],settingWindowName);
        limits.maxH = cv::getTrackbarPos(TrackbarName[1],settingWindowName);
        limits.minS = cv::getTrackbarPos(TrackbarName[2],settingWindowName);
        limits.maxS = cv::getTrackbarPos(TrackbarName[3],settingWindowName);
        limits.minV = cv::getTrackbarPos(TrackbarName[4],settingWindowName);
        limits.maxV = cv::getTrackbarPos(TrackbarName[5],settingWindowName);
        minArea = cv::getTrackbarPos(TrackbarName[6],settingWindowName);
        maxArea = cv::getTrackbarPos(TrackbarName[7],settingWindowName);;
        minRoundness = cv::getTrackbarPos(TrackbarName[8],settingWindowName);
    }
    char TrackbarName[9][50] = { "MinH","MaxH","MinS","MaxS","MinV","MaxV","MinArea","MaxArea","MinRound" };
public:
    cv::Point_<int> location, lastLocation, vectorXY, predictedLocation;
    std::vector<std::vector<cv::Point_<int>>> trajs;


    int vectorMult = 20;
    int walls[4][3];
    int goalArr[2][3];
    int predicted[3];
    double det;
    cv::Point_<double> intersect;

    bool bouncex;
    bool bouncey;
    bool found;
    int lostCnt;
    bool goalFlag;
    std::vector<cv::Point_<int>> Goals;


    bool onTable = false;

    Thing();
    ~Thing() = default;
    void setupTrackbars();
    double getMinArea();
    double getMaxArea();
    double getMinRoundness();
    std::vector<cv::Point_<int>> find(cv::Mat in, Table table);    //Changed from int to pointer, returns coords
    void findOld(cv::Mat in, Table table, bool isMallet);    //Changed from int to pointer, returns coords
    void calcVector(cv::Mat in);
    void drawVector(cv::Mat in);
    void calcTraj(Table table);
    void toggleDebugInfo() {
        debugWindows = !debugWindows;
        if(debugWindows) {
            setupTrackbars();
            cv::namedWindow(previewWindowName,0);
        }
        else {
            cv::destroyWindow(settingWindowName);
            cv::destroyWindow(previewWindowName);
        }
    }
    void setGoals(std::vector<cv::Point_<int>> sortedX);
    void calcTrajNew(Table table, cv::Mat grabbed);
    std::vector<bool> bounceDetect(Table table, cv::Point_<int> startPoint, cv::Point_<int> endPoint, cv::Mat grabbed);
    cv::Point_<int> findIntersection(std::vector<bool> bounces, cv::Point_<int> startPoint, cv::Point_<int> endPoint);

};

#endif //TEAM555_THING_H

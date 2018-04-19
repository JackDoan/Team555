//
// Created by jad140230 on 3/17/2018.
//

#ifndef TEAM555_THING_H
#define TEAM555_THING_H


#include <opencv2/opencv.hpp>
#include "../Table.h"
#include "History.h"
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
    bool doBars = false;
    char* settingWindowName = const_cast<char *>("Settings");
    char* previewWindowName = const_cast<char *>("Thing");
    bool debug = false;
    bool debugWindows = false;
    struct threshold_s limits;
    cv::Mat getThresholdImage(cv::Mat& in);
    cv::Scalar outlineColor = cv::Scalar(40, 255, 255);
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
    int historyDepth;

    void goalDetect(cv::Point_<int> intersection, int xvelo);
    void goalDetectOffense(cv::Point_<int> intersection, int xvelo);


public:
    cv::Point_<int> location, lastLocation, vectorXY, predictedLocation;


    cv::Point_<int> L_mid;
    cv::Point_<int> R_mid;
    clock_t s;
    clock_t e;
    int vectorMult = 15;
    double walls[4][3];
//    int goalArr[2][3];
//    int predicted[3];
//    double det;
    cv::Point_<double> intersect;

    bool bouncex;
    bool bouncey;
    bool found;
    int lostCnt;
    bool goalFlag;
    bool leftGoal;
    bool rightGoal;
    bool leftGoalOffense;
    bool rightGoalOffense;
    std::vector<cv::Point_<int>> Goals;
    std::vector<std::vector<cv::Point_<int>>> trajectory;



    bool onTable = false;

    Thing();
    ~Thing() = default;
    void setupTrackbars();
    double getMinArea();
    double getMaxArea();
    double getMinRoundness();
    std::vector<cv::Point_<int>> find(cv::Mat& in);    //Changed from int to pointer, returns coords
    void findOne(cv::Mat in, bool isMallet);    //Changed from int to pointer, returns coords
    void calcVector(cv::Mat& in);

    double magHistoryAvg;

    void toggleDebugInfo() {
        debugWindows = !debugWindows;
        if(debugWindows) {
            doBars = true;
            //cv::namedWindow(previewWindowName,0);
        }
        else {
            cv::destroyWindow(settingWindowName);
            //cv::destroyWindow(previewWindowName);
        }
    }
    void setGoals(std::vector<cv::Point_<int>> sortedX);
    void drawTraj(cv::Mat& in, std::vector<std::vector<cv::Point_<int>>> traj);
    std::vector<std::vector<cv::Point_<int>>> calcTraj(cv::Mat grabbed, cv::Point_<int> lastLoc, cv::Point_<int> loc);
    std::vector<std::vector<cv::Point_<int>>> calcTrajOffense(cv::Mat& grabbed, cv::Point_<int> lastLoc, cv::Point_<int> loc);
    std::vector<bool> bounceDetect(cv::Point_<int> startPoint, cv::Point_<int> endPoint, cv::Mat grabbed, int bnccnt);
    cv::Point_<int> findIntersection(std::vector<bool> bounces, cv::Point_<int> startPoint, cv::Point_<int> endPoint);


    std::vector<bool> foundHistory;
    std::vector<double> magHistory;

    std::vector<bool> rightGoalHistory;
    std::vector<bool> leftGoalHistory;

    void fillFoundHistory(bool found);

    History<cv::Point_<int>> locHist = History<cv::Point_<int>>(cv::Scalar(50, 255, 200), 3);



    std::vector<std::vector<std::vector<cv::Point_<int>>>> trajectoryHistory;
    bool drawWholeHistory;
    void fillTrajHistory();
    void drawTrajHistory(cv::Mat& in);
    void drawTrajEndPointHistory(cv::Mat& in);
    void drawGoalVector(cv::Mat& in);
    //    void drawGoalVector(cv::Mat& in, std::vector<cv::Point_<int>> sortedY);


    void fillVeloMagHistory();
    void writeVeloMagHistory(cv::Mat& in);

    void fillGoalFlagsHistory();
    void calcNextLoc();
    cv::Point_<int> predictLocation(int frames);
    std::vector<bool> bounceDetectClean(cv::Point_<int> startPoint, cv::Point_<int> endPoint, int bnccnt);

};



#endif //TEAM555_THING_H

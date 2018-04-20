//
// Created by jad140230 on 3/17/2018.
//

#ifndef TEAM555_THING_H
#define TEAM555_THING_H


#include <opencv2/opencv.hpp>
#include "../Table.h"
#include "../Settings.h"
#include "History.h"
#include <vector>
#include <math.h>
#include <thread>
#include <string>



class Thing {
protected:

    bool& doBars;

    char* settingWindowName = const_cast<char *>("Settings");
    char* previewWindowName = const_cast<char *>("Thing");
    bool debug = false;
    bool debugWindows = false;
    cv::Mat getThresholdImage(cv::Mat& in);
    cv::Scalar outlineColor = cv::Scalar(40, 255, 255);
    static void onChange(int v, void *ptr) {
        // resolve 'this':
        auto *that = (Thing*)ptr;
        that->realTrack(v);
    }
    struct threshold_s* limits;
    int* minArea;
    int* maxArea;
    int* minRoundness;
    void realTrack(int v) {
        limits->minH = cv::getTrackbarPos(TrackbarName[0],settingWindowName);
        limits->maxH = cv::getTrackbarPos(TrackbarName[1],settingWindowName);
        limits->minS = cv::getTrackbarPos(TrackbarName[2],settingWindowName);
        limits->maxS = cv::getTrackbarPos(TrackbarName[3],settingWindowName);
        limits->minV = cv::getTrackbarPos(TrackbarName[4],settingWindowName);
        limits->maxV = cv::getTrackbarPos(TrackbarName[5],settingWindowName);
        *minArea = cv::getTrackbarPos(TrackbarName[6],settingWindowName);
        *maxArea = cv::getTrackbarPos(TrackbarName[7],settingWindowName);;
        *minRoundness = cv::getTrackbarPos(TrackbarName[8],settingWindowName);
    }
    char TrackbarName[9][50] = { "MinH","MaxH","MinS","MaxS","MinV","MaxV","MinArea","MaxArea","MinRound" };
    int historyDepth;

    void goalDetectOffense(cv::Point_<int> intersection, int xvelo);


public:
    cv::Point_<int> location, lastLocation, vectorXY;


    clock_t s;
    clock_t e;
    int vectorMult = 15;
    double walls[4][3];


    bool found;
    int lostCnt;
    bool leftGoal;
    bool rightGoal;
    bool leftGoalOffense;
    bool rightGoalOffense;

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

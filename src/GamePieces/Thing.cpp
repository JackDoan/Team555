////
//// Created by jad140230 on 3/17/2018.
////
//
//
//#include "../../inc/Table.h"
//#include "../../inc/helpers.h"
//#include "../../inc/Settings.h"
//
//#include <vector>
//#include <math.h>
//#include <thread>
//#include <ctime>
//#include <string>
//
//
//
//void Thing::calcVector(cv::Mat& in) {
//
//    vectorXY = location - lastLocation;
//    /*if (abs(location.x-lastLocation.x) < 3 && abs(location.y - lastLocation.y) < 3) {
//        vectorXY = {0,0};
//    } else {
//        vectorXY = location - lastLocation;
//    }*/
//
//}
//
//
//
//
//void Thing::drawTraj(cv::Mat& in, std::vector<std::vector<cv::Point_<int>>> traj) {
//    for (int i = 0; i < traj.size(); i++) {
//        cv::line(in, traj[i][0], traj[i][1], cvScalar(165, 255, 255), 4);
//    }
//}
//
//
//
//
//
//
//void Thing::drawTrajHistory(cv::Mat& in) {
//    for (int i = historyDepth/3-1; i > 0; i--) {
//        if (foundHistory[i]) {
//            for (int j = 0; j < trajectoryHistory[i].size(); j++) {
//                cv::line(in, trajectoryHistory[i][j][0], trajectoryHistory[i][j][1], cvScalar(165, 255, 255), 4);
//            }
//            if (!drawWholeHistory) {
//                break;
//            }
//        }
//    }
//}
//
//void Thing::drawTrajEndPointHistory(cv::Mat& in) {
//    for (int i = 1; i < historyDepth/3; i++) {
//        if (foundHistory[i-1] && foundHistory[i]) {
//            cv::circle(in, trajectoryHistory[i-1].back().back(), 10, cv::Scalar(150, 255, 100 - i*5), 3);
//        }
//    }
//}
//
//void Thing::fillVeloMagHistory() {
//    e = clock();
//    double el = double(e - s) / CLOCKS_PER_SEC;
//    // magnitude of pixels per second (px/sec)
//    magHistory.insert(magHistory.begin(), sqrt(pow(vectorXY.x, 2) + pow(vectorXY.y, 2)) / el);
//    magHistory.resize(historyDepth);
//    s = clock();
//    //magHistoryAvg = meanVector(magHistory);
//}
//
//
//
//void Thing::writeVeloMagHistory(cv::Mat& in) {
//    char tempStr[80] = {};
//    for (int i = 0; i < historyDepth; i++) {
//        sprintf(tempStr, "Velo %d: %5.2f", i, magHistory[i]);
//        cv::putText(in, tempStr, cvPoint(10, 100 + 40 * i), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(225, 225, 0), 3);
//    }
//}
//
//void Thing::fillGoalFlagsHistory() {
//    rightGoalHistory.insert(rightGoalHistory.begin(), rightGoal);
//    rightGoalHistory.resize(historyDepth);
//    leftGoalHistory.insert(leftGoalHistory.begin(), leftGoal);
//    leftGoalHistory.resize(historyDepth);
//}
//
//void Thing::drawGoalVector(cv::Mat& in){
//
//    cv::Point_<int> tempGoal = {30, 685/2};
//    cv::line(in, tempGoal, location, cvScalar(20, 200, 20), 4);
//
//    //To eventually replace shotSpot
//    //Todo: apply calcTraj(maybe?) to allow for bounce shots
//    float shotSpotScalar = 0.2;
//    cv::Point_<int> JshotSpot = {shotSpotScalar*(location.x - 30) + location.x, shotSpotScalar*(location.y - (685/2)) + location.y};
//
////    cv::Point_<int> malletMaxY = {0, (sortedY[3].y - 80)};      //todo: adjust to ratios
////    cv::Point_<int> malletMinY = {0, (sortedY[0].y + 80)};
////    cv::Point_<int> malletMinY = Table::motionLimitMin;
////
////
////    if(JshotSpot.y > malletMaxY.y){
////        JshotSpot.y = malletMaxY.y;
////    }
////    else if (JshotSpot.y < malletMinY.y){
////        JshotSpot.y = malletMinY.y;
////    }
//
//
//    cv::line(in, location, JshotSpot, cvScalar(20,20,20), 4);
//    cv::circle(in, JshotSpot, 10, cv::Scalar(15, 15, 15), 6);
//
//}
//
//
//// TODO: we can use this to detect an alias, set an alias flag high in the next frame and avoid calculating a bad trajectory
//void Thing::calcNextLoc() {
//    // really want to calculate a predicted location in the next frame
//    cv::Point_<int> total;
//    total.x = abs(location.x - lastLocation.x);
//    total.y = abs(location.y - lastLocation.y);
//    if(total.x > abs(trajectory.front()[1].x) || total.y > abs(trajectory.front()[1].y)) {
//        cv::Point_<int> leftover;
//        leftover.x = total.x - abs(trajectory.front()[1].x);
//        leftover.y = total.y - abs(trajectory.front()[1].y);
//        // idk how to finish this, seems complicated
//    }
//}
//
//

//
// Created by jad140230 on 3/17/2018.
//

#include "../inc/Thing.h"
#include "../inc/Table.h"
#include "../inc/helpers.h"
#include <vector>
#include <math.h>
#include <thread>
#include <ctime>
#include <string>




void Thing::setupTrackbars() {
    doBars = false;
        int slidermax = 255;
        int sslidermax = 5000;
        cv::namedWindow(settingWindowName, 1);
        cv::createTrackbar(TrackbarName[0], settingWindowName, &(limits.minH), slidermax, onChange, this);
        cv::createTrackbar(TrackbarName[1], settingWindowName, &(limits.maxH), slidermax, onChange, this);
        cv::createTrackbar(TrackbarName[2], settingWindowName, &(limits.minS), slidermax, onChange, this);
        cv::createTrackbar(TrackbarName[3], settingWindowName, &(limits.maxS), slidermax, onChange, this);
        cv::createTrackbar(TrackbarName[4], settingWindowName, &(limits.minV), slidermax, onChange, this);
        cv::createTrackbar(TrackbarName[5], settingWindowName, &(limits.maxV), slidermax, onChange, this);
        cv::createTrackbar(TrackbarName[6], settingWindowName, &minArea, sslidermax, onChange, this);
        cv::createTrackbar(TrackbarName[7], settingWindowName, &maxArea, sslidermax, onChange, this);
        cv::createTrackbar(TrackbarName[8], settingWindowName, &minRoundness, sslidermax, onChange, this);
        //cv::resizeWindow(settingWindowName, 500, 800);
        //TableCalibrate.setupTrackbars();
}

cv::Mat Thing::getThresholdImage(cv::Mat& in) {

    cv::Mat blurred,result;
    cv::GaussianBlur(in, blurred, cv::Size(3, 3), 0, 0); //smooth the original image using Gaussian kernel

    //cv::cvtColor(blurred, imgHSV, CV_BGR2HSV); //Change the color format from BGR to HSV
    //todo relable HSV things to BGR things

    cv::inRange(blurred,
                cv::Scalar(limits.minH, limits.minS, limits.minV),
                cv::Scalar(limits.maxH, limits.maxS, limits.maxV),
                result);

    return result;
}

// TODO: add logic to puck.findPuck that uses functions from camera, to adjust exposure and gain when puck isn't found for a long time
// If speed is measured 'high' and puck is not found, turn up exposure and gain until it is found again
// then once it is found again return it to its default setting

double Thing::getMinArea() {
    return (double)minArea;
}

double Thing::getMaxArea() {
    return (double)maxArea;
}

double Thing::getMinRoundness() {
    return (double)(minRoundness);
}

Thing::Thing() {

    struct threshold_s limitsCalib;
    //for green

    //    limits.minH = 43;
    //    limits.maxH = 110;
    //    limits.minS = 70;
    //    limits.maxS = 155;
    //    limits.minV = 20;
    //    limits.maxV = 67;
    limits.minH = 16;
    limits.maxH = 121;
    limits.minS = 23;
    limits.maxS = 185;
    limits.minV = 0;
    limits.maxV = 0;

    limitsCalib.minH = 43;
    limitsCalib.maxH = 110;
    limitsCalib.minS = 70;
    limitsCalib.maxS = 155;
    limitsCalib.minV = 20;
    limitsCalib.maxV = 70;
    //for orange
    /*
    limits.minH = 39;
    limits.maxH = 103;
    limits.minS = 90;
    limits.maxS = 149;
    limits.minV = 121;
    limits.maxV = 212;
     */

    s = clock();
    e = clock();
    location = {0,0};
    lastLocation = {0,0};
    vectorXY = {0,0};
    predictedLocation = {0,0};
    bouncex = false;
    bouncey = false;
    found = false;
    lostCnt = 0;
    historyDepth = 15;
    for (int i = 0; i < historyDepth; i++) {
        foundHistory.insert(foundHistory.begin(), false);
    }
    drawWholeHistory = false;
}

std::vector<cv::Point_<int>> Thing::find(cv::Mat in, Table table) {
    double area = 0;
    double perimeter = 0;
    double roundness = 0;
    int num;
    std::vector<cv::Point_<int>> pointVec;
//    pointVec.reserve(4);


    cv::Mat imgThresh = getThresholdImage(in);
    cv::Mat imgThreshSmall;
    cv::resize(imgThresh,imgThreshSmall, cv::Size(), 0.25, 0.25);
    if (debugWindows) {
        imshow("Puck", imgThreshSmall);
    }
    std::vector<std::vector<cv::Point> > contours;  //hold the pointer to a contour in the memory block
    CvSeq* result;   //hold sequence of points of a contour
    //CvMemStorage *storage = cvCreateMemStorage(0); //storage area for all contours

    // Position initialization
    cv::Point_<int> pos = {0,0};
    //double localLastX = x;
    //double localLastY = y;
    num = 0;

    cv::InputArray thisone = imgThresh;
    cv::findContours(imgThresh, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    for (int i = 0; i< contours.size(); i++) {
        //todo: shrink the image to the table area, THEN search for contours.
        num++;
        area = cv::contourArea(contours[i]);
        //printf("%s %.2f %.2f", logStr, area, roundness);
        //sprintf(logStr,"%s;%d %.2f", logStr,num, area);
//        if ((area>getMinArea()) && (area<getMaxArea())) {  // Min and Max size of object
        if ((area>(double)minAreaCalib) && (area<(double)maxAreaCalib)) {
            //Detecting roundness   roundness = perimeter^2 / (2*pi*area)
            perimeter = cv::arcLength(contours[i], true);
            roundness = (perimeter*perimeter) / (6.28*area);
            if (roundness < (double)minRoundnessCalib) {
                cv::Moments moments = cv::moments(contours[i], true);
                double moment10 = moments.m10;
                double moment01 = moments.m01;
                area = moments.m00;
                // Calculate object center
                pos.x = (int)(moment10*2 / area);
                pos.y = (int)(moment01*2 / area);

                //pos.x = floor(moments.m10 * 2 / area + 0.5); // round
                //pos.y = floor(moments.m01 * 2 / area + 0.5);

                // limit the region of interest to the table
//                if ((pos.x > table.max.x * 2) ||
//                    (pos.x < table.min.x * 2) ||
//                    (pos.y > table.max.y * 2) ||
//                    (pos.y < table.min.y * 2)) {
//                    pos.x = 0;
//                    pos.y = 0;
//                    //contours = contours->h_next;
//                    continue;  // continue with other contour... (this is outside the table)
//                }
//                else {
                //location = pos;

//                if (pointVec.size() > 0) {
//                    printf("%15d, %15d,\t", (int)round(pos.x/2), (int)round(pos.y/2));
//                }
                pointVec.emplace_back((int)round(pos.x/2), (int)round(pos.y/2));
/*                for (int j = 0; j < pointVec.size(); j++) {
                    if (pointVec.size() == 4)
                        printf("Number: %d: x: %15d,\ty: %15d\n",j , pointVec[j].x, pointVec[j].y);
                }*/
//                if (pointVec.size() > 0) {
//                    printf("%15d, %15d,\t %15d\n", pointVec[pointVec.size()].x, pointVec[pointVec.size()].y, i);
//                }
//                }

                // Draw contour
                if (table.preview == 1) {
                    cv::drawContours(in, contours, i, cv::Scalar(255, 255, 255 ), 5, 8);
                    //printf("I GOT TO THE CONTOURS PRINTING THING!!\n\nYAY!!\n");
                }
//                   { switch (i) {
//                        case 1:
//                            cv::drawContours(in, contours, i, cv::Scalar(255, 0, 255), 5, 8);
//                        case 2:
//                            cv::drawContours(in, contours, i, cv::Scalar(255, 0, 0), 5, 8);
//                        case 3:
//                            cv::drawContours(in, contours, i, cv::Scalar(0, 255, 0), 5, 8);
//                        case 4:
//                            cv::drawContours(in, contours, i, cv::Scalar(0, 0, 255), 5, 8);
//                    }
//                }

            }
        }
    }

    return pointVec;
}

void Thing::findOne(cv::Mat in, Table table, bool isMallet) {
    onTable = false;
    goalFlag = false;
    trajectory.clear();
    leftGoal = false;
    rightGoal = false;

    double area = 0;
    double perimeter = 0;
    double roundness = 0;

    cv::Mat imgThresh = getThresholdImage(in);
    if (debugWindows) {
        setupTrackbars();
        cv::Mat imgThreshSmall;
        cv::resize(imgThresh, imgThreshSmall, cv::Size(), 0.25, 0.25);
        imshow(settingWindowName, imgThreshSmall);
    }
    std::vector< std::vector<cv::Point> > contours;  //hold the pointer to a contour in the memory block
    CvSeq* result;   //hold sequence of points of a contour

//    Coordinate pos = Coordinate(0.0,0);
    cv::Point_<int> pos = {0,0};

    cv::findContours(imgThresh, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    found = false;
    lastLocation = location;
    for (int i = 0; i < contours.size(); i++) {
        //todo: shrink the image to the table area, THEN search for contours.
        area = cv::contourArea(contours[i]);
        if ((area > getMinArea()) && (area < getMaxArea())) {  // Min and Max size of object
            //Detecting roundness   roundness = perimeter^2 / (2*pi*area)
            perimeter = cv::arcLength(contours[i], true);
            roundness = (perimeter * perimeter) / (6.28 * area);
            if (roundness < getMinRoundness()) {
                cv::Moments moments = cv::moments(contours[i], true);
                double moment10 = moments.m10;
                double moment01 = moments.m01;
                area = moments.m00;
                // Calculate object center
                pos.x = moment10 * 2 / area;
                pos.y = moment01 * 2 / area;


                // limit the region of interest to the table
                if ((pos.x > table.max.x * 2) ||
                    (pos.x < table.min.x * 2) ||
                    (pos.y > table.max.y * 2) ||
                    (pos.y < table.min.y * 2)) {
                    pos.x = 0;
                    pos.y = 0;
                    continue;  // continue with other contour... (this is outside the table)
                } else {
                    location = pos/2;

                    // Draw contour
                    if (table.preview == 1)
                        cv::drawContours(in, contours, i, outlineColor, 5, 4);
                    found = true;
                    break;
                }

            }
        }
    }


    if (!found) {
        lostCnt++;
        if (lostCnt < 10) {
            //lastLocation = location;
            location = location + vectorXY;
        }

    }

// TODO: this needs to be improved, this if statement resets location and lost count
    // because the 'predicted' last location can't totally be trusted after the puck reappears
    // need to make it so if where the puck reappears is 'accurate' then use the previous
    // predicted location as its last location and continue and if not reset and do NOT use
    // the last predicted location as last location

    if ((found && lostCnt > 0)) {
        lostCnt = 0;
    } else {
        calcVector(in);
        if (!isMallet) {
            trajectory = calcTraj(table, in);
            predictedLocation = trajectory.back()[1];
            drawTraj(in);
//            calcTraj(table);
//            drawVector(in);
        }
    }

    onTable = within(location, table.min, table.max);


/*    if (predictedLocation.x < 0 || predictedLocation.y < 0) {
        printf("Location: (%d, %d)\n"
                       "lastLocation: (%d, %d)\n"
                       "intersect: (%f, %f)\n"
                       "predcitedLocation: (%d, %d)\n",
               location.x, location.y,
               lastLocation.x, location.y,
               intersect.x, intersect.y,
               predictedLocation.x, predictedLocation.y);
    }
    printf("Location: (%d, %d)\n"
                   "lastLocation: (%d, %d)\n"
                   "intersect: (%f, %f)\n"
                   "predcitedLocation: (%d, %d)\n",
           location.x, location.y,
           lastLocation.x, location.y,
           intersect.x, intersect.y,
           predictedLocation.x, predictedLocation.y);
    if (predictedLocation.x > table.max.x || predictedLocation.x < table.min.x ||
            predictedLocation.y > table.max.y || predictedLocation.y < table.min.y) {
        printf("broke\n");
    }*/
    if (!isMallet) {
        fillFoundHistory(found);
        fillLocationHistory(location);
        drawLocationHistory(in);
        fillTrajHistory();
//        drawTrajHistory(in);
        drawTrajEndPointHistory(in);
//        fillVeloMagHistory();
//        writeVeloMagHistory(in);
        fillGoalFlagsHistory();
        //drawGoalVector(in);               todo: convince Mike to use this
        //drawGoalVector(in, corners.sortedY);
    }


}

void Thing::calcVector(cv::Mat in) {
/*    // TODO: move this into calcTraj
    // Calculate speed and angle
//    Vector VectorXY;



//test
//    if (location.x >= 1.001 * lastLocation.x || location.x <= 0.999 * lastLocation.x) {
//                printf("coordx,y: %d, %d\t\t lastcoordx,y: %d, %d\n", location.x,
//                          location.y, lastLocation.x, lastLocation.y);
//                printf("vectorx,y: %d, %d\n", vectorXY.x, vectorXY.y);
//    }
//   if (vectorX != 0 && vectorY != 0)
//    if (vectorXY.x != 0 || vectorXY.y != 0) {
////       printf("vectorX: %d\n", vectorX);
////       printf("vectorY: %d\n", vectorY);
////        cv::line(in, cvPoint(lastLocation.x/2, lastLocation.y/2), cvPoint(location.x/2 + vectorXY.x*vectorMult/2,
////                                                                      location.y/2 + vectorXY.y*vectorMult/2),
////                                                                       cvScalar(255, 0, 255), 4);
//        cv::line(in, location, (location+vectorXY*vectorMult), cvScalar(255, 0, 255), 4);
//
//
//
//    } else {
//    //        printf("coord: %f\n", location.x);
//    //        printf("lastCoord %f\n", lastLocation.x);
//        //printf("coordY: %f\n", location.y);
//        //printf("lastCoordY %f\n", lastLocation.y);
//    }
//    //    VectorXY = Vector(vectorX, vectorY);*/

    if (abs(location.x-lastLocation.x) < 3 && abs(location.y - lastLocation.y) < 3) {
        vectorXY = {0,0};
    } else {
        vectorXY = location - lastLocation;
    }

}


// TODO: programatically determine the mid points from corners, why 720/2 and 700/2?
void Thing::setGoals(std::vector<cv::Point_<int>> sortedX){
    int goalScale = 50;
    int goalPush = 0;
    Goals = {cvPoint(0, 0), cvPoint(0, 0), cvPoint(0, 0), cvPoint(0, 0)};

    cv::Point_<int> L_mid = {sortedX[1].x, 685/2};
    cv::Point_<int> R_mid = {sortedX[2].x, 725/2};

    cv::Point_<int> L_top = {L_mid.x, L_mid.y + 45 + goalScale};
    cv::Point_<int> L_bottom = {L_mid.x, L_mid.y - 45 - goalScale};

    cv::Point_<int> R_top = {R_mid.x+goalPush, R_mid.y + 45 + goalScale};
    cv::Point_<int> R_bottom = {R_mid.x+goalPush, R_mid.y - 45 - goalScale};

    //cv::line(previewSmall, L_top/2, L_bottom/2, cv::Scalar(255, 0, 0), 4);
    //cv::line(previewSmall, R_top/2, R_bottom/2, cv::Scalar(255, 0, 0), 4);

    Goals[0] = L_top;
    Goals[1] = L_bottom;
    Goals[2] = R_top;
    Goals[3] = R_bottom;
}

void Thing::drawTraj(cv::Mat in) {
    for (int i = 0; i < trajectory.size(); i++) {
        cv::line(in, trajectory[i][0], trajectory[i][1], cvScalar(255, 0, 255), 4);
    }
}

std::vector<std::vector<cv::Point_<int>>> Thing::calcTraj(Table table, cv::Mat grabbed) {
    // need to set left and right goal flags to false at the start of this
    // remove grabbed once done testing and debugging
    intersect = {0, 0};
    cv::Point_<int> dist;
    cv::Point_<int> leftover;
    std::vector<std::vector<cv::Point_<int>>> trajs;
    int bnccnt = 0;
    int bnccntmax;
    if (lostCnt > 0) {
        bnccntmax = 1;
    } else {
        bnccntmax = 3;
    }


    cv::Point_<int> prediction;

    prediction = location + vectorXY*vectorMult;


    std::vector<cv::Point_<int>> temp;
    temp.emplace_back(location);
    temp.emplace_back(prediction);
    trajs.emplace_back(temp);
   /* char tempStr[80];
    sprintf(tempStr, "Leg: %d = (%d, %d) -> (%d, %d)\n", -1, trajs.back()[0].x, trajs.back()[0].y, trajs.back()[1].x, trajs.back()[1].y);
    cv::putText(grabbed, tempStr, cvPoint(30, 410), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(225, 225, 50), 2);*/
//    std::vector<bool> bounces = {false, false, false, false};

    while (bnccnt < bnccntmax) {
        std::vector<bool> bounces = {false, false, false, false};
        cv::Point_<int> intersection;
        cv::Point_<int> newEndPoint;
        // detect a bounce
        bounces = bounceDetect(table, trajs.back()[0], trajs.back()[1], grabbed, bnccnt);
        // if all of bounces are false, set done = true and break
        if (bounces[0] || bounces[1] || bounces[2] || bounces[3]) {
            // determine from bounces which wall is going to be intersected
            // and calculate the intersection point
            intersection = findIntersection(bounces, trajs.back()[0], trajs.back()[1]);

            // draw the intersection on grabbed, delete this after testing and debugging is finished
/*
            cv::line(grabbed, cvPoint(intersection.x-25, intersection.y),
                     cvPoint(intersection.x+25, intersection.y),
                     cvScalar(0, 0, 255), 4);
            cv::line(grabbed, cvPoint(intersection.x, intersection.y-25),
                     cvPoint(intersection.x, intersection.y+25),
                     cvScalar(0, 0, 255), 4);
*/

            // determine whether or not this is a goal
            // depending on which goal is getting intersected
            if (bounces[0] || bounces[2]) {
                goalDetect(intersection, trajs.back()[1].x - trajs.back()[0].x);
            }
            if (leftGoal || rightGoal) {
                // set endPoint equal to intersection and break
                trajs.back()[1] = intersection;
                break;
            }

            // determine distance from location to intersect point, this becomes the length of the current leg
            // subtract that from the original length and this becomes the length of the next leg
            dist = cvPoint(abs(intersection.x - trajs.back()[0].x),
                           abs(intersection.y - trajs.back()[0].y));
            leftover = cvPoint(abs(trajs.back()[1].x - dist.x),
                               abs(trajs.back()[1].y - dist.y));
//             determine ratios of leftover/total
            double xrat = ((double)leftover.x)/((double)abs(trajs.back()[1].x - trajs.back()[0].x));
            double yrat = ((double)leftover.y)/((double)abs(trajs.back()[1].y - trajs.back()[0].y));
            double magorig = sqrt(pow(trajs.back()[1].x - trajs.back()[0].x, 2) +
                                    pow(trajs.back()[1].y - trajs.back()[0].y, 2));
            double magclipped = sqrt(pow(intersection.x - trajs.back()[0].x, 2) +
                                     pow(intersection.y - trajs.back()[0].y, 2));
            double magrat = magclipped / magorig;

//             emplace back next trajs, which has start point at the point of intersect
            if (bounces[0] || bounces[2]) {
                newEndPoint.x = (int)(round((trajs.back()[0].x - trajs.back()[1].x) * (1 - magrat))) + intersection.x;
                newEndPoint.y = (int)(round((trajs.back()[1].y - trajs.back()[0].y) * (1 - magrat))) + intersection.y;
            } else if (bounces[1] || bounces[3]) {
                newEndPoint.x = (int)(round((trajs.back()[1].x - trajs.back()[0].x) * (1 - magrat))) + intersection.x;
                newEndPoint.y = (int)(round((trajs.back()[0].y - trajs.back()[1].y) * (1 - magrat))) + intersection.y;
            }
            // and appropriate endpoint

            // set trajs.back()[1] to the point of intersection
            trajs.back()[1] = intersection;
            std::vector<cv::Point_<int>> tmptmp;
            tmptmp.emplace_back(intersection);
            tmptmp.emplace_back(newEndPoint);
            trajs.emplace_back(tmptmp);

        }
        /*char tempStr[80];
        sprintf(tempStr, "Leg: %d = (%d, %d) -> (%d, %d)\n", bnccnt+1, trajs.back()[0].x, trajs.back()[0].y, trajs.back()[1].x, trajs.back()[1].y);
        cv::putText(grabbed, tempStr, cvPoint(30, 450 + 40 * bnccnt+1), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(225, 225, 50), 2);*/
        bnccnt++;
    }

    return trajs;
}
std::vector<bool> Thing::bounceDetect(Table table, cv::Point_<int> startPoint, cv::Point_<int> endPoint, cv::Mat grabbed, int bnccnt) {
    //remove grabbed onced done testing and debugging

    std::vector<bool> output = {false, false, false, false};
    int truecnt = 0;


     if (endPoint.x < table.min.x) {
         output[0] = true;
         truecnt++;
//         printf("0\n");
     } else if (endPoint.x > table.max.x) {
         output[2] = true;
         truecnt++;
//         printf("2\n");
     }

     // if there is an intersect with a horizontal wall (bottom and top walls)
     if (endPoint.y > table.max.y) {
         output[1] = true;
         truecnt++;
//         printf("1\n");
     } else if (endPoint.y < table.min.y) {
         output[3] = true;
         truecnt++;
//         printf("3\n");
     }

     // need to check if both a vertical and horizontal bounce are detected

     if (truecnt > 2) {
        printf("bounceDetect error, truecnt >2\n");
        return {false, false, false, false};
     }
//    char tempStr[80];
    if (truecnt > 1) {
        // checking the top left condition
        if (output[0] && output[3]) {
            int xdif = startPoint.x - table.min.x;
            int xvelo = abs(endPoint.x - startPoint.x);
            double xtime = (double)xdif/(double)xvelo;
            int ydif = startPoint.y - table.min.y;
            int yvelo = abs(endPoint.y - startPoint.y);
            double ytime = (double)ydif/(double)yvelo;
            /*sprintf(tempStr,"(xtime, ytime): (%f, %f) walls: (%s, %s, %s, %s)\n", xtime, ytime,
                                    output[0]?"1":"0", output[1]?"1":"0",
                                    output[2]?"1":"0", output[3]?"1":"0");
            cv::putText(grabbed, tempStr, cvPoint(30, 80  + 40 * bnccnt), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(225, 225, 50), 2);*/
            if (ytime > xtime) {
                output[3] = false;
            } else {
                output[0] = false;
            }
        }
        // checking the bottom left condition
        if (output[0] && output[1]) {
            int xdif = startPoint.x - table.min.x;
            int xvelo = abs(endPoint.x - startPoint.x);
            double xtime = (double)xdif/(double)xvelo;
            int ydif = table.max.y - startPoint.y;
            int yvelo = abs(endPoint.y - startPoint.y);
            double ytime = (double)ydif/(double)yvelo;
            /*sprintf(tempStr,"(xtime, ytime): (%f, %f) walls: (%s, %s, %s, %s)\n", xtime, ytime,
                    output[0]?"1":"0", output[1]?"1":"0",
                    output[2]?"1":"0", output[3]?"1":"0");
            cv::putText(grabbed, tempStr, cvPoint(30, 80 + 40 * bnccnt), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(225, 225, 50), 2);*/
            if (ytime > xtime) {
                output[1] = false;
            } else {
                output[0] = false;
            }
        }
        // checking the bottom right condition
        if (output[1] && output[2]) {
            int xdif = table.max.x - startPoint.x;
            int xvelo = abs(endPoint.x - startPoint.x);
            double xtime = (double)xdif/(double)xvelo;
            int ydif = table.max.y - startPoint.y;
            int yvelo = abs(endPoint.y - startPoint.y);
            double ytime = (double)ydif/(double)yvelo;
            /*sprintf(tempStr,"(xtime, ytime): (%f, %f) walls: (%s, %s, %s, %s)\n", xtime, ytime,
                    output[0]?"1":"0", output[1]?"1":"0",
                    output[2]?"1":"0", output[3]?"1":"0");
            cv::putText(grabbed, tempStr, cvPoint(30, 80 + 40 * bnccnt), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(225, 225, 50), 2);*/
            if (ytime > xtime) {
                output[1] = false;
            } else {
                output[2] = false;
            }
        }
        // checking the top right condition
        if (output[2] && output[3]) {
            int xdif = table.max.x - startPoint.x;
            int xvelo = abs(endPoint.x - startPoint.x);
            double xtime = (double)xdif/(double)xvelo;
            int ydif = startPoint.y - table.min.y;
            int yvelo = abs(endPoint.y - startPoint.y);
            double ytime = (double)ydif/(double)yvelo;
            /*sprintf(tempStr,"(xtime, ytime): (%f, %f) walls: (%s, %s, %s, %s)\n", xtime, ytime,
                    output[0]?"1":"0", output[1]?"1":"0",
                    output[2]?"1":"0", output[3]?"1":"0");
            cv::putText(grabbed, tempStr, cvPoint(30, 80 + 40 * bnccnt), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(225, 225, 50), 2);*/
            if (ytime > xtime) {
                output[3] = false;
            } else {
                output[2] = false;
            }
        }

    }
    return output;

}


cv::Point_<int> Thing::findIntersection(std::vector<bool> bounces, cv::Point_<int> startPoint, cv::Point_<int> endPoint) {
    cv::Point_<int> intersection = {0, 0};
    double prediction[3];
    double determinant;
    prediction[0] = endPoint.y - startPoint.y;    //A
    prediction[1] = startPoint.x - endPoint.x;    //B
    prediction[2] = prediction[0]*startPoint.x + prediction[1]*startPoint.y;   //C

    // if we are intersecting wall 0 (left)
    if (bounces[0]) {
        determinant = prediction[0] * walls[0][1] - walls[0][0] * prediction[1];
        if (determinant == 0) {
            //printf("Errors lines are parallel?\n");
        } else {
            double tempx = (walls[0][1] * prediction[2] - prediction[1] * walls[0][2])/determinant;
            intersection.x = (int)tempx;
            double tempy = (prediction[0] * walls[0][2] - walls[0][0] * prediction[2])/determinant;
            intersection.y = (int)tempy;
//            intersection.x = (walls[0][1] * prediction[2] - prediction[1] * walls[0][2])/determinant;
//            intersection.y = (prediction[0] * walls[0][2] - walls[0][0] * prediction[2])/determinant;
        }
    }
    // if we are intersecting wall 1 (bottom)
    if (bounces[1]) {
        determinant = prediction[0] * walls[1][1] - walls[1][0] * prediction[1];
        if (determinant == 0) {
            //printf("Errors lines are parallel?\n");
        } else {
            double tempx = (walls[1][1] * prediction[2] - prediction[1] * walls[1][2])/determinant;
            intersection.x = (int)tempx;
            double tempy = (prediction[0] * walls[1][2] - walls[1][0] * prediction[2])/determinant;
            intersection.y = (int)tempy;
//            intersection.x = (walls[1][1] * prediction[2] - prediction[1] * walls[1][2])/determinant;
//            intersection.y = (prediction[0] * walls[1][2] - walls[1][0] * prediction[2])/determinant;
        }
    }
    // if we are intersecting wall 2 (right)
    if (bounces[2]) {
        determinant = prediction[0] * walls[2][1] - walls[2][0] * prediction[1];
        if (determinant == 0) {
            //printf("Errors lines are parallel?\n");
        } else {
            double tempx = (walls[2][1] * prediction[2] - prediction[1] * walls[2][2])/determinant;
            intersection.x = (int)tempx;
            double tempy = (prediction[0] * walls[2][2] - walls[2][0] * prediction[2])/determinant;
            intersection.y = (int)tempy;
//            intersection.x = (walls[2][1] * prediction[2] - prediction[1] * walls[2][2])/determinant;
//            intersection.y = (prediction[0] * walls[2][2] - walls[2][0] * prediction[2])/determinant;
        }
    }
    // if we are intersecting wall 3 (top)
    if (bounces[3]) {
        determinant = prediction[0] * walls[3][1] - walls[3][0] * prediction[1];
        if (determinant == 0) {
            //printf("Errors lines are parallel?\n");
        } else {
            double tempx = (walls[3][1] * prediction[2] - prediction[1] * walls[3][2])/determinant;
            intersection.x = (int)tempx;
            double tempy = (prediction[0] * walls[3][2] - walls[3][0] * prediction[2])/determinant;
            intersection.y = (int)tempy;
//            intersection.x = (walls[3][1] * prediction[2] - prediction[1] * walls[3][2])/determinant;
//            intersection.y = (prediction[0] * walls[3][2] - walls[3][0] * prediction[2])/determinant;
        }
    }
    return intersection;
}

void Thing::goalDetect(cv::Point_<int> intersection, int xvelo) {
    if(Goals.size() < 4) {
        return;
    }
    if (xvelo > 0 && intersection.y >= Goals[3].y && intersection.y <= Goals[2].y) {
        rightGoal = true;
    } else if (xvelo < 0 && intersection.y >= Goals[1].y && intersection.y <= Goals[2].y) {
        leftGoal = true;
    }
};

void Thing::fillFoundHistory(bool found) {
    foundHistory.insert(foundHistory.begin(), found);
    foundHistory.resize(historyDepth);
}

void Thing::fillLocationHistory(cv::Point_<int>) {
    locationHistory.insert(locationHistory.begin(), location);
    locationHistory.resize(historyDepth);
}

void Thing::drawLocationHistory(cv::Mat in){
    for (int i = 0; i < historyDepth; i++) {
        if (foundHistory[i])
            cv::circle(in, locationHistory[i], 10, cv::Scalar(100, 200 - i*10, 0), 6);
    }
}

void Thing::fillTrajHistory(){
    trajectoryHistory.insert(trajectoryHistory.begin(), trajectory);
    trajectoryHistory.resize(historyDepth);
}

void Thing::drawTrajHistory(cv::Mat in) {
    for (int i = historyDepth-1; i > 0; i--) {
        if (foundHistory[i]) {
            for (int j = 0; j < trajectoryHistory[i].size(); j++) {
                cv::line(in, trajectoryHistory[i][j][0], trajectoryHistory[i][j][1], cvScalar(210, 0, 210), 4);
            }
            if (!drawWholeHistory) {
                break;
            }
        }
    }
}

void Thing::drawTrajEndPointHistory(cv::Mat in) {
    for (int i = 1; i < historyDepth; i++) {
        if (foundHistory[i-1] && foundHistory[i]) {
            cv::circle(in, trajectoryHistory[i-1].back().back(), 10, cv::Scalar(210, 0, 210), 6);
        }
    }
}

void Thing::fillVeloMagHistory() {
    e = clock();
    double el = double(e - s) / CLOCKS_PER_SEC;
    // magnitude of pixels per second (px/sec)
    magHistory.insert(magHistory.begin(), sqrt(pow(vectorXY.x, 2) + pow(vectorXY.y, 2)) / el);
    magHistory.resize(historyDepth);
    s = clock();
}

void Thing::writeVeloMagHistory(cv::Mat in) {
    char tempStr[80] = {};
    for (int i = 0; i < historyDepth; i++) {
        sprintf(tempStr, "Velo %d: %5.2f", i, magHistory[i]);
        cv::putText(in, tempStr, cvPoint(10, 100 + 40 * i), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(225, 225, 0), 3);
    }
}

void Thing::fillGoalFlagsHistory() {
    rightGoalHistory.insert(rightGoalHistory.begin(), rightGoal);
    rightGoalHistory.resize(historyDepth);
    leftGoalHistory.insert(leftGoalHistory.begin(), leftGoal);
    leftGoalHistory.resize(historyDepth);
}

void Thing::drawGoalVector(cv::Mat in){

    cv::Point_<int> tempGoal = {30, 685/2};
    cv::line(in, tempGoal, location, cvScalar(20, 200, 20), 4);

    //To eventually replace shotSpot
    //Todo: apply calcTraj(maybe?) to allow for bounce shots
    float shotSpotScalar = 0.2;
    cv::Point_<int> JshotSpot = {shotSpotScalar*(location.x - 30) + location.x, shotSpotScalar*(location.y - (685/2)) + location.y};

    //cv::Point_<int> malletMaxY = {0, (sortedY[3].y - 80)};      //todo: adjust to ratios
    //cv::Point_<int> malletMinY = {0, (sortedY[0].y + 80)};

//    if(JshotSpot.y > malletMaxY.y){
//        JshotSpot.y = malletMaxY.y;
//    }
//    else if (JshotSpot.y < malletMinY.y){
//        JshotSpot.y = malletMinY.y;
//    }

    cv::line(in, location, JshotSpot, cvScalar(20,20,20), 4);
    cv::circle(in, JshotSpot, 10, cv::Scalar(15, 15, 15), 6);

}


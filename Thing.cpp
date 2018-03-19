//
// Created by jad140230 on 3/17/2018.
//

#include "inc/Thing.h"
#include "inc/Table.h"
#include "inc/helpers.h"
#include <vector>
#include <math.h>
#include <thread>
#include <ctime>




void Thing::setupTrackbars() {
    if(debugWindows) {
        int slidermax = 255;
        int sslidermax = 5000;
        cv::namedWindow(settingWindowName, 0);
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

    location = {0,0};
    lastLocation = {0,0};
    vectorXY = {0,0};
    predictedLocation = {0,0};
    bouncex = false;
    bouncey = false;
    found = false;
    lostCnt = 0;

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
    imshow("Puck", imgThreshSmall);
    std::vector<std::vector<cv::Point> > contours;  //hold the pointer to a contour in the memory block
    CvSeq* result;   //hold sequence of points of a contour
    //CvMemStorage *storage = cvCreateMemStorage(0); //storage area for all contours

    // Position initialization
    //double posX = 0;
    //double posY = 0;
    Coordinate pos = Coordinate(0.0,0);
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
                pos.x = moment10*2 / area;
                pos.y = moment01*2 / area;

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

void Thing::findOld(cv::Mat in, Table table, bool isMallet) {
    onTable = false;
    goalFlag = false;

    double area = 0;
    double perimeter = 0;
    double roundness = 0;

    cv::Mat imgThresh = getThresholdImage(in);
    if (debugWindows) {
        cv::Mat imgThreshSmall;
        cv::resize(imgThresh, imgThreshSmall, cv::Size(), 0.25, 0.25);
        imshow(previewWindowName, imgThreshSmall);
    }
    std::vector< std::vector<cv::Point> > contours;  //hold the pointer to a contour in the memory block
    CvSeq* result;   //hold sequence of points of a contour

//    Coordinate pos = Coordinate(0.0,0);
    cv::Point_<int> pos = {0,0};

    cv::findContours(imgThresh, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    found = false;
    lastLocation = location; // move this out of the for loop, seems dumb? -Mike
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
            lastLocation = location;
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
            calcTraj(table);
            drawVector(in);
        }

    }

    if (location.x < table.max.x && location.y < table.max.y && location.x > table.min.x && location.y > table.min.y)  {
        onTable = true;
    }


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
    vectorXY.x = (location.x - lastLocation.x);
    vectorXY.y = (location.y - lastLocation.y);

}

void Thing::drawVector(cv::Mat in) {

    if (bouncex || bouncey) {
//                printf("Bounce was true! and intersect\n");
        cv::line(in, cvPoint(predictedLocation.x-5, predictedLocation.y),
                     cvPoint(predictedLocation.x+5, predictedLocation.y),
                     cvScalar(0, 0, 255), 4);
        cv::line(in, cvPoint(predictedLocation.x, predictedLocation.y-5),
                     cvPoint(predictedLocation.x, predictedLocation.y+5),
                     cvScalar(0, 0, 255), 4);
        cv::line(in, location, intersect, cvScalar(255, 0, 255), 4);
        cv::line(in, intersect, predictedLocation, cvScalar(255, 225, 0), 4);

//        if (bouncex) {
//            predictedLocation.x = intersect.x - vectorXY.x * vectorMult;
//            predictedLocation.y = intersect.y + vectorXY.y * vectorMult;
//            cv::line(in, intersect, predictedLocation, cvScalar(255, 225, 0), 4);
//        } else if (bouncey) {
//            predictedLocation.x = intersect.x + vectorXY.x * vectorMult;
//            predictedLocation.y = intersect.y - vectorXY.y * vectorMult;
//            cv::line(in, intersect, predictedLocation, cvScalar(255, 225, 0), 4);
//        }

    } else {
        cv::line(in, location, (location+vectorXY*vectorMult), cvScalar(255, 0, 255), 4);
    }

    cv::circle(in, predictedLocation, 20, outlineColor, 4);


    //printf("No puck found!\n");

}

void Thing::calcTraj(Table table) {
    predictedLocation = location + vectorXY*vectorMult;
    predicted[0] = predictedLocation.y - location.y;    //A
    predicted[1] = location.x - predictedLocation.x;    //B
    predicted[2] = predicted[0]*location.x + predicted[1]*location.y;   //C
    if (
            predictedLocation.x > table.max.x &&
                (
                    abs(location.y - table.max.y) > abs(location.x - table.max.x) &&
                    abs(location.y - table.min.y) > abs(location.x - table.max.x)
                )
            )
    {
        bouncex = true;
        bouncey = false; //testing this
        // third wall
        det = predicted[0] * walls[2][1] - walls[2][0] * predicted[1];
        if (det == 0) {
            //dbgPrint("Error lines are parallel?\n");
        }
        else {
            intersect.x = (walls[2][1] * predicted[2] - predicted[1] * walls[2][2]) / det;
            intersect.y = (predicted[0] * walls[2][2] - walls[2][0] * predicted[2]) / det;
//            printf("The intersect point is: (%f, %f)\n", intersect.x, intersect.y);
        }
    }
    else if (
            predictedLocation.x < table.min.x &&
            (abs(location.y - table.max.y) > abs(location.x - table.min.x) &&
             abs(location.y - table.min.y) > abs(location.x - table.min.x))
            ) {
        bouncex = true;
        bouncey = false; //testing this
        // first wall
        det = predicted[0] * walls[0][1] - walls[0][0] * predicted[1];
        if (det == 0) {
            //dbgPrint("Error lines are parallel?\n");
        }
        else {
            intersect.x = (walls[0][1] * predicted[2] - predicted[1] * walls[0][2])/det;
            intersect.y = (predicted[0] * walls[0][2] - walls[0][0] * predicted[2])/det;
//            printf("The intersect point is: (%f, %f)\n", intersect.x, intersect.y);
        }
    }
    else if (
            predictedLocation.y > table.max.y &&
            (abs(location.x - table.max.x) > abs(location.y - table.max.y) &&
             abs(location.x - table.min.x) > abs(location.y - table.max.y))
            ) {
        bouncey = true;
        bouncex = false;
        // second wall
        det = predicted[0] * walls[1][1] - walls[1][0] * predicted[1];
        if (det == 0) {
            //printf("Error lines are parallel?\n");
        }
        else {
            intersect.x = (walls[1][1] * predicted[2] - predicted[1] * walls[1][2])/det;
            intersect.y = (predicted[0] * walls[1][2] - walls[1][0] * predicted[2])/det;
//           printf("The intersect point is: (%f, %f)\n", intersect.x, intersect.y);
        }
    }
    else if (
            predictedLocation.y < table.min.y &&
            (abs(location.x - table.max.x) > abs(location.y - table.min.y) &&
             abs(location.x - table.min.x) > abs(location.y - table.min.y))
            ) {
        bouncey = true;
        bouncex = false;
        // fourth wall
        det = predicted[0] * walls[3][1] - walls[3][0] * predicted[1];
        if (det == 0) {
        //            printf("Error lines are parallel?\n");
        }
        else {
            intersect.x = (walls[3][1] * predicted[2] - predicted[1] * walls[3][2])/det;
            intersect.y = (predicted[0] * walls[3][2] - walls[3][0] * predicted[2])/det;
//            printf("The intersect point is: (%f, %f)\n", intersect.x, intersect.y);
        }
    }
    else {
        bouncex = false;
        bouncey = false;
    }



    if (bouncex || bouncey) {
    //                printf("Bounce was true! and intersect\n");
        // bouncex means you are intersecting a y wall
        if (bouncex && !bouncey) {
            if (intersect.y >= Goals[1].y || intersect.y <= Goals[0].y || intersect.y >= Goals[3].y || intersect.y <= Goals[2].y) {
                goalFlag = true;
                predictedLocation = intersect;
            } else {
                predictedLocation.x = intersect.x - vectorXY.x * vectorMult;
                predictedLocation.y = intersect.y + vectorXY.y * vectorMult;
            }
        }
            // bouncy means you are intersecting a x wall
        else if (bouncey && !bouncex) {
            predictedLocation.x = intersect.x + vectorXY.x * vectorMult;
            predictedLocation.y = intersect.y - vectorXY.y * vectorMult;
        }
    }

}

void Thing::setGoals(std::vector<cv::Point_<int>> sortedX){

    Goals = {cvPoint(0, 0), cvPoint(0, 0), cvPoint(0, 0), cvPoint(0, 0)};

    cv::Point_<int> L_mid = {sortedX[0].x, 700/2};
    cv::Point_<int> R_mid = {sortedX[3].x, 720/2};

    cv::Point_<int> L_top = {L_mid.x, L_mid.y + 90};
    cv::Point_<int> L_bottom = {L_mid.x, L_mid.y - 90};

    cv::Point_<int> R_top = {R_mid.x, R_mid.y + 90};
    cv::Point_<int> R_bottom = {R_mid.x, R_mid.y - 90};

    //cv::line(previewSmall, L_top/2, L_bottom/2, cv::Scalar(255, 0, 0), 4);
    //cv::line(previewSmall, R_top/2, R_bottom/2, cv::Scalar(255, 0, 0), 4);
    Goals[0] = L_top;
    Goals[1] = L_bottom;
    Goals[2] = R_top;
    Goals[3] = R_bottom;
}


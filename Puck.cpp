//
// Created by jad140230 on 1/21/2018.
//

#include "Puck.h"
#include "ThresholdImage.h"
#include "Table.h"
#include "helpers.h"
#include <vector>
#include <math.h>
#include <thread>
#include <ctime>


void Puck::setupTrackbars() {
    char TrackbarName1[50];
    char TrackbarName2[50];
    char TrackbarName3[50];

    char name[] = "Puck Settings";
    int slidermax = 5000;
    sprintf(TrackbarName1, "MinArea %d", slidermax);
    sprintf(TrackbarName2, "MaxArea %d", slidermax);
    sprintf(TrackbarName3, "MinRound %d", slidermax);

    cv::namedWindow(name, 1);

    cv::createTrackbar(TrackbarName1, name, &minArea, slidermax, nullptr);
    cv::createTrackbar(TrackbarName2, name, &maxArea, slidermax, nullptr);
    cv::createTrackbar(TrackbarName3, name, &minRoundness, slidermax, nullptr);
    thresholdImage.setupTrackbars("Puck Color Settings");
    //TableCalibrate.setupTrackbars();
}

// TODO: add logic to puck.findPuck that uses functions from camera, to adjust exposure and gain when puck isn't found for a long time
// If speed is measured 'high' and puck is not found, turn up exposure and gain until it is found again
// then once it is found again return it to its default setting

double Puck::getMinArea()
{
    return (double)minArea;
}

double Puck::getMaxArea()
{
    return (double)maxArea;
}

double Puck::getMinRoundness()
{
    return (double)(minRoundness);
}

Puck::Puck(Table table) {
    struct threshold_s puckLimits;
    struct threshold_s puckLimitsCalib;
    //for green

//    puckLimits.minH = 43;
//    puckLimits.maxH = 110;
//    puckLimits.minS = 70;
//    puckLimits.maxS = 155;
//    puckLimits.minV = 20;
//    puckLimits.maxV = 67;
    puckLimits.minH = 16;
    puckLimits.maxH = 121;
    puckLimits.minS = 23;
    puckLimits.maxS = 185;
    puckLimits.minV = 0;
    puckLimits.maxV = 0;

    puckLimitsCalib.minH = 43;
    puckLimitsCalib.maxH = 110;
    puckLimitsCalib.minS = 70;
    puckLimitsCalib.maxS = 155;
    puckLimitsCalib.minV = 20;
    puckLimitsCalib.maxV = 70;



    //for orange
    /*
    puckLimits.minH = 39;
    puckLimits.maxH = 103;
    puckLimits.minS = 90;
    puckLimits.maxS = 149;
    puckLimits.minV = 121;
    puckLimits.maxV = 212;
     */

    thresholdImage = ThresholdImage(puckLimits);
    thresholdImageCalib = ThresholdImage(puckLimitsCalib);
    location = {0,0};
    lastLocation = {0,0};
    vectorXY = {0,0};
    predictedLocation = {0,0};
    bouncex = false;
    bouncey = false;
    puckFound = false;
    lostCnt = 0;

}

Puck::~Puck() = default;

std::vector<cv::Point_<int>> Puck::findPucks(cv::Mat in, Table table) {
    double area = 0;
    double perimeter = 0;
    double roundness = 0;
    int num;
    std::vector<cv::Point_<int>> pointVec;
//    pointVec.reserve(4);


    cv::Mat imgThresh = thresholdImageCalib.get(in);
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

//void Puck::find(cv::Mat in, Table table) {
void Puck::findPuck(cv::Mat in, Table table) {


    double area = 0;
    double perimeter = 0;
    double roundness = 0;
    int num;

//    cv::Point_<int> puckPoint;


    cv::Mat imgThresh = thresholdImage.get(in);
    cv::Mat imgThreshSmall;

    cv::resize(imgThresh,imgThreshSmall, cv::Size(), 0.25, 0.25);
    imshow("Puck", imgThreshSmall);
    std::vector< std::vector<cv::Point> > contours;  //hold the pointer to a contour in the memory block
    CvSeq* result;   //hold sequence of points of a contour
    //CvMemStorage *storage = cvCreateMemStorage(0); //storage area for all contours

    //cv::Mat imgCorners = TableCalibrate.get(in);
    //mshow("Corners", imgCorners);

    // Position initialization
    //double posX = 0;
    //double posY = 0;
//    Coordinate pos = Coordinate(0.0,0);
    cv::Point_<int> pos = {0,0};
//    Coordinate lastPos = Coordinate(0,0);
    //double localLastX = x;
    //double localLastY = y;
    num = 0;

    cv::InputArray thisone = imgThresh;
    cv::findContours(imgThresh, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    puckFound = false;
    lastLocation = location; // move this out of the for loop, seems dumb? -Mike
    for (int i = 0; i < contours.size(); i++) {
        //todo: shrink the image to the table area, THEN search for contours.
        num++;
        area = cv::contourArea(contours[i]);
        //printf("%s %.2f %.2f", logStr, area, roundness);
        //sprintf(logStr,"%s;%d %.2f", logStr,num, area);
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

                //pos.x = floor(moments.m10 * 2 / area + 0.5); // round
                //pos.y = floor(moments.m01 * 2 / area + 0.5);

                // limit the region of interest to the table
                if ((pos.x > table.max.x * 2) ||
                    (pos.x < table.min.x * 2) ||
                    (pos.y > table.max.y * 2) ||
                    (pos.y < table.min.y * 2)) {
                    pos.x = 0;
                    pos.y = 0;
                    //contours = contours->h_next;
                    continue;  // continue with other contour... (this is outside the table)
                } else {
                    location = pos/2;
//                    puckPoint.x = (int) round(pos.x);
//                    puckPoint.y = (int) round(pos.y);
//                }
//                    printf("puckPoint: %d, %d\n", (int) round(pos.x), (int) round(pos.y));
//                    printf("lastLocation: %d, %d\n", lastLocation.x, lastLocation.y);
//                    printf("location: %d, %d\n", location.x, location.y);


                    // Draw contour
                    if (table.preview == 1)
                        cv::drawContours(in, contours, i, cv::Scalar(0, 255, 0), 5, 4);

                    puckFound = true;
                    break;
                }

            }
        }
    }

    //puck.speedX = vectorX * 100 / time;  // speed in dm/ms (
    //puck.speedY = vectorY * 100 / time;

    //CoordsDouble = Coordinate(location.x/2 - table.cam_center_x, location.y/2 - table.cam_center_y);
    //CoordsDouble = Coordinate(location.x/2, location.y/2);

    if (!puckFound) {
        lostCnt++;
        if (lostCnt < 5) {
            lastLocation = location;
            location = location + vectorXY;
        }

    }

    // TODO: need to make two separate paths in calcVector, calcTraj and drawVector
    // One for when the puck is found and when the puck is not found
    // when puck is found use location and last location
    // when puc is not found use estimated location and estimated last location
    // also need to make a counter for how many frames the puck is not found
    // if it goes more than 5 frames without finding the puck then stop calculating
    // and drawing the trajectories and vectors

    if (!(puckFound && lostCnt > 0)) {
//        calcVector(in);
        calcTraj(table);
        drawVector(in);
    } else {
        lostCnt = 0;
    }




}

void Puck::calcVector(cv::Mat in) {
    // TODO: move this into calcTraj
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
//    //    VectorXY = Vector(vectorX, vectorY);

}

void Puck::drawVector(cv::Mat in) {

    if (bouncex || bouncey) {
//                printf("Bounce was true! and intersect\n");
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

    cv::circle(in, predictedLocation, 20, cvScalar(0, 255, 0), 4);


        //printf("No puck found!\n");

}

void Puck::calcTraj(Table table) {
    vectorXY.x = (location.x - lastLocation.x);
    vectorXY.y = (location.y - lastLocation.y);
    predictedLocation = location + vectorXY*vectorMult;
    predicted[0] = predictedLocation.y - location.y;    //A
    predicted[1] = location.x - predictedLocation.x;    //B
    predicted[2] = predicted[0]*location.x + predicted[1]*location.y;   //C
    if (predictedLocation.x > table.max.x && (abs(location.y - table.max.y) > abs(location.x - table.max.x) && abs(location.y - table.min.y) > abs(location.x - table.max.x))) {
//    if (predictedLocation.x > table.max.x) {
          bouncex = true;
          // third wall
          det = predicted[0] * walls[2][1] - walls[2][0] * predicted[1];
          if (det == 0) {
//            printf("Error lines are parallel?\n");
          } else {
              intersect.x = (walls[2][1] * predicted[2] - predicted[1] * walls[2][2]) / det;
              intersect.y = (predicted[0] * walls[2][2] - walls[2][0] * predicted[2]) / det;
//            printf("The intersect point is: (%f, %f)\n", intersect.x, intersect.y);
          }
    } else if (predictedLocation.x < table.min.x && (abs(location.y - table.max.y) > abs(location.x - table.min.x) && abs(location.y - table.min.y) > abs(location.x - table.min.x))) {
//    } else if (predictedLocation.x < table.min.x) {
        bouncex = true;
        // first wall
        det = predicted[0] * walls[0][1] - walls[0][0] * predicted[1];
        if (det == 0) {
//            printf("Error lines are parallel?\n");
        } else {
            intersect.x = (walls[0][1] * predicted[2] - predicted[1] * walls[0][2])/det;
            intersect.y = (predicted[0] * walls[0][2] - walls[0][0] * predicted[2])/det;
//            printf("The intersect point is: (%f, %f)\n", intersect.x, intersect.y);
        }
    } else if (predictedLocation.y > table.max.y && (abs(location.x - table.max.x) > abs(location.y - table.max.y) && abs(location.x - table.min.x) > abs(location.y - table.max.y))) {
        bouncey = true;
        // second wall
        det = predicted[0] * walls[1][1] - walls[1][0] * predicted[1];
        if (det == 0) {
//            printf("Error lines are parallel?\n");
        } else {
            intersect.x = (walls[1][1] * predicted[2] - predicted[1] * walls[1][2])/det;
            intersect.y = (predicted[0] * walls[1][2] - walls[1][0] * predicted[2])/det;
//            printf("The intersect point is: (%f, %f)\n", intersect.x, intersect.y);
        }
    } else if (predictedLocation.y < table.min.y && (abs(location.x - table.max.x) > abs(location.y - table.min.y) && abs(location.x - table.min.x) > abs(location.y - table.min.y))) {
        bouncey = true;
        // fourth wall
        det = predicted[0] * walls[3][1] - walls[3][0] * predicted[1];
        if (det == 0) {
//            printf("Error lines are parallel?\n");
        } else {
            intersect.x = (walls[3][1] * predicted[2] - predicted[1] * walls[3][2])/det;
            intersect.y = (predicted[0] * walls[3][2] - walls[3][0] * predicted[2])/det;
//            printf("The intersect point is: (%f, %f)\n", intersect.x, intersect.y);
        }
    } else {
        bouncex = false;
        bouncey = false;
    }

    if (bouncex || bouncey) {
//                printf("Bounce was true! and intersect\n");
        if (bouncex && !bouncey) {
            predictedLocation.x = intersect.x - vectorXY.x * vectorMult;
            predictedLocation.y = intersect.y + vectorXY.y * vectorMult;
        } else if (bouncey && !bouncex) {
            predictedLocation.x = intersect.x + vectorXY.x * vectorMult;
            predictedLocation.y = intersect.y - vectorXY.y * vectorMult;
        }

    }
    // TODO: Calculate a final predicted location with a bounce
//    if (predictedLocation.y > table.max.y || predictedLocation.y < table.min.y) {
//        printf("ERROR bounce prediction messed up!\n");
//        printf("Coords: %d, %d\n", location.x, location.y);
//        printf("Predicted Coords: %d, %d\n", predictedLocation.x, predictedLocation.y);
//        printf("Vector: %d, %d\n", vectorXY.x, vectorXY.y);
//        if (bouncex) {
//            printf("Bouncex was true!\n");
//        } else if (bouncey) {
//            printf("Bouncey was true!\n");
//        }
//    }

}


void Puck::setWalls(std::vector<cv::Point_<int>> sortedX, std::vector<cv::Point_<int>> sortedY) {
   /*
    * This calculates the Ax + By = C line equations for the walls of the table
    * A = y2-y1, B = x1-x2, C = A*x1 + B*y1
    */

    // Calculate the left (first) wall with the two minimum x corners
    walls[0][0] = sortedX[1].y - sortedX[0].y;
    walls[0][1] = sortedX[0].x - sortedX[1].x;
    walls[0][2] = walls[0][0]*sortedX[0].x + walls[0][1]*sortedX[0].y;

    // Calculate the bottom (second) wall with the two maximum y corners
    walls[1][0] = sortedY[3].y - sortedY[2].y;
    walls[1][1] = sortedY[2].x - sortedY[3].x;
    walls[1][2] = walls[1][0]*sortedY[2].x + walls[1][1]*sortedY[2].y;

    // Calculate the right (third) wall with the two maximum x corners
    walls[2][0] = sortedX[3].y - sortedX[2].y;
    walls[2][1] = sortedX[2].x - sortedX[3].x;
    walls[2][2] = walls[2][0]*sortedX[2].x + walls[2][1]*sortedX[2].y;

    // Calculate the top (fourth) wall with the two minimum y corners
    walls[3][0] = sortedY[1].y - sortedY[0].y;
    walls[3][1] = sortedY[0].x - sortedY[1].x;
    walls[3][2] = walls[3][0]*sortedY[0].x + walls[3][1]*sortedY[0].y;
}

void Puck::setGoals(cv::Mat previewSmall, std::vector<cv::Point_<int>> sortedX){
    cv::Point_<int> L_mid;
    cv::Point_<int> R_mid;

    L_mid.y = sortedX[3].y - sortedX[2].y;
    if(L_mid.y < 0)
    {
        L_mid.y = sortedX[2].y - sortedX[3].y;
    }
    L_mid.x = (sortedX[3].x + sortedX[2].x) / 2;

    R_mid.y = sortedX[1].y - sortedX[0].y;
    if(R_mid.y < 0)
    {
        R_mid.y = sortedX[0].y - sortedX[1].y;
    }
    R_mid.x = (sortedX[1].x + sortedX[0].x) / 2;

    cv::Point_<int> L_top = {L_mid.x, L_mid.y + 100};
    cv::Point_<int> L_bottom = {L_mid.x, L_mid.y - 100};

    cv::Point_<int> R_top = {R_mid.x, R_mid.y + 100};
    cv::Point_<int> R_bottom = {R_mid.x, R_mid.y - 100};

    cv::line(previewSmall, L_top/2, L_bottom/2, cv::Scalar(255, 0, 0), 4);

    cv::line(previewSmall, R_top/2, R_bottom/2, cv::Scalar(255, 0, 0), 4);

    printf("L  x: %d, L y: %d\n", L_mid.x/2, L_mid.y/2);
    printf("R  x: %d, R y: %d\n", R_mid.x/2, R_mid.y/2);
}
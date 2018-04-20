//
// Created by jad140230 on 1/21/2018.
//

#include "../../inc/GamePieces/Puck.h"
#include "../../inc/Table.h"
#include "../../inc/helpers.h"
#include <vector>
#include <math.h>
#include <thread>
#include <ctime>


// TODO: add logic to puck.findPuck that uses functions from camera, to adjust exposure and gain when puck isn't found for a long time
// If speed is measured 'high' and puck is not found, turn up exposure and gain until it is found again
// then once it is found again return it to its default setting

Puck::Puck() {

    //for green


    settingWindowName = const_cast<char *>("Puck Settings");
    previewWindowName = const_cast<char *>("Puck");

    lostCnt = 0;

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


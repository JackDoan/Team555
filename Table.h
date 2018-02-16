//
// Created by jad140230 on 1/21/2018.
//

#ifndef AIRHOCKEYROBOT_TABLE_H
#define AIRHOCKEYROBOT_TABLE_H


// All units in milimeters?

#include "Camera.h"
#include "helpers.h"


#define CAM_PIX_WIDTH 1280
#define CAM_PIX_HEIGHT 720
#define CAM_PIX_TO_MM 1  // Default camera distante (this is rewrited on the configuration)

class Table {
public:

    int fps = 60; //used to define the FPS frames are saved at
    int robot_table_length = 1930; //mm
    int robot_table_width = 1200; //mm
    int robot_table_center_x = robot_table_length/2;
    int robot_table_center_y = robot_table_width /2;
    int puckSize = 0;  // Puck size (radio) estimation. it depends on table size (width)

    int defense_position;
    int predict_x;    // X position at impact (mm)
    int predict_y;
    int predict_x_old;
    int predict_y_old;
    int predict_time;   // time to impact in ms

    Coordinate max, min;

    int cam_center_x = CAM_PIX_WIDTH/2;
    int cam_center_y = CAM_PIX_HEIGHT/2;

    //int cam_center_x = CAM_PIX_WIDTH/2;
    //int cam_center_y = CAM_PIX_HEIGHT/2;

    float cam_pix_to_mm = CAM_PIX_TO_MM; //todo replace this with an expression
    int preview = 1;

    void annotate(cv::Mat);
    Table(Camera camera);
    ~Table();
};

#endif //AIRHOCKEYROBOT_TABLE_H

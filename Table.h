//
// Created by jad140230 on 1/21/2018.
//

#ifndef AIRHOCKEYROBOT_TABLE_H
#define AIRHOCKEYROBOT_TABLE_H

//todo identify table corners programmatically
//todo All units in pixels?

#define CAM_PIX_WIDTH 1280
#define CAM_PIX_HEIGHT 720
#define CAM_PIX_TO_MM 1

class Table {
public:


    int fps = 60; //used to define the FPS frames are saved at
    int robot_table_length = 1930; //mm
    int robot_table_width = 1200; //mm
    int robot_table_center_x = robot_table_length/2;
    int robot_table_center_y = robot_table_width /2;
    int puckSize = 0;  // Puck size (radio) estimation. it depends on table size (width)

    char logStr[4096];

    int defense_position;
    int predict_x;    // X position at impact (mm)
    int predict_y;
    int predict_x_old;
    int predict_y_old;
    int predict_time;   // time to impact in ms
    char puckDataString2[80];


    // variables to draw the table
    int table_pix_minx;
    int table_pix_miny;
    int table_pix_maxx;
    int table_pix_maxy;

    int cam_center_x;
    int cam_center_y;
    float cam_pix_to_mm = CAM_PIX_TO_MM; //todo replace this with an expression
    int preview = 1;

    Table(int length, int width);

    Table();
    ~Table();
};

#endif //AIRHOCKEYROBOT_TABLE_H

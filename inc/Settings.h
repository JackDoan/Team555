//
// Created by mdl150330 on 3/15/2018.
//

#ifndef TEAM555_SETTINGS_H
#define TEAM555_SETTINGS_H

struct threshold_s {
    int minH = 26;
    int maxH = 128;
    int minS = 69;
    int maxS = 138;
    int minV = 59;
    int maxV = 172;
    int minArea = 100;
    int maxArea = 10000;
    int minRoundness = 100;
    bool doBars = true;
    cv::Scalar outlineColor = cv::Scalar(40, 255, 255);;
};

class Settings {
private:
public:
    static bool video_output;   //can be shown through Idle Process
    static bool undistort;
    static bool calibrateCorners;
    static bool preview;

    static struct threshold_s puckLimits = {
            .minH = 31,
            .maxH = 61,
            .minS = 53,
            .maxS = 160,
            .minV = 0,
            .maxV = 255,
            .minArea = 1200,
            .maxArea = 3600,
            .minRoundness = 100,
            .doBars = false,
            .outlineColor = cv::Scalar(40, 255, 255),
    };

    static struct threshold_s malletLimits = {
            .minH = 63,
            .maxH = 109,
            .minS = 80,
            .maxS = 203,
            .minV = 0,
            .maxV = 255,
            .minArea = 1548,
            .maxArea = 4500,
            .minRoundness = 800,
            .doBars = false,
            .outlineColor = cv::Scalar(210, 255, 255),
    };

    static bool threadFindingThings;



};
#endif //TEAM555_SETTINGS_H

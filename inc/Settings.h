//
// Created by mdl150330 on 3/15/2018.
//

#ifndef TEAM555_SETTINGS_H
#define TEAM555_SETTINGS_H

struct threshold_s {
    explicit threshold_s(bool isMallet) {
        if(!isMallet) {
            minH = 31;
            maxH = 61;
            minS = 53;
            maxS = 160;
            minV = 0;
            maxV = 255;
            minArea = 1200;
            maxArea = 3600;
            minRoundness = 100;
            doBars = true;
            outlineColor = cv::Scalar(40, 255, 255);
            windowName[4] = '\0';
        }
        else {
            minH = 63;
            maxH = 109;
            minS = 80;
            maxS = 203;
            minV = 0;
            maxV = 255;
            minArea = 1100;
            maxArea = 4500;
            minRoundness = 800;
            doBars = true;
            //debug = true;
            outlineColor = cv::Scalar(210, 255, 255);

        }

    }

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
    bool debug = false;
    cv::Scalar outlineColor = cv::Scalar(40, 255, 255);
    char windowName[16] = "Mallet";
};

class Settings {
private:
    static bool processLine(std::fstream &f, char *buffer, unsigned int bufSize);
    static std::string cameraUndistortString;
    static std::string malletLimitString;
    static std::string puckLimitString;
    static std::string cornerString;
    static char delim;
    static std::string filePath;
    static void writeThreshold(const threshold_s& t, std::fstream& f);
    static void readThreshold(threshold_s& t, std::fstream& f);

public:
    static bool video_output;   //can be shown through Idle Process
    static bool undistort;

    static struct threshold_s puckLimits;
    static struct threshold_s malletLimits;
    static bool threadFindingThings;

    static bool readConfigValues(const std::string& path);
    static bool readConfigValues() { return readConfigValues(filePath); }
    static bool writeConfigValues(const std::string& path);
    static bool writeConfigValues() { return writeConfigValues(filePath); }


};
#endif //TEAM555_SETTINGS_H

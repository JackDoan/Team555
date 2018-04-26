//
// Created by jes130330 on 3/8/2018.
//
#include "../inc/Corners.h"
#include "../inc/Config.h"
#include "../inc/Settings.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
bool Settings::video_output = false;   //can be shown through Idle Process
bool Settings::undistort = true;
bool Settings::calibrateCorners = false;
bool Settings::preview = true;

struct threshold_s Settings::puckLimits = threshold_s(false);
struct threshold_s Settings::malletLimits = threshold_s(true);
bool Settings::threadFindingThings = !(puckLimits.debug & malletLimits.debug);

string Settings::cameraUndistortString = "camera.undistort";
string Settings::malletLimitString = "mallet.limits";
string Settings::puckLimitString = "puck.limits";
string Settings::cornerString = "table.corners";
string Settings::filePath = "config.txt";
char Settings::delim = ':';


bool Settings::processLine(fstream &f, char *buffer, unsigned int bufSize) {
    f.getline(buffer, bufSize, delim); //get a line, look for a colon
    //begin error checking:
    if(!f.good()) {
        if(f.eof()) {
            cout << "Error! A setting is named but not defined in the config file. Check for extra colons." << endl;
        }
        else {
            cout << "Error! Something really bad happened to your config file." << endl;
        }
        return false;
    }
        //end error checking
    else { //things were good!
        if(
                buffer[0] == '/' ||
                buffer[0] == '#' ||
                buffer[0] == '\\') {
            //then this was a comment
            return true;
        }
        else if(strncmp(buffer, cameraUndistortString.c_str(), bufSize) == 0) {
            f.get(); //throw out a ':'
            f >> skipws >> boolalpha >> Settings::undistort;
        }
        else if(strncmp(buffer, malletLimitString.c_str(), bufSize) == 0) {
            f.get(); //throw out a ':'
            readThreshold(malletLimits, f);
        }
        else if(strncmp(buffer,puckLimitString.c_str() , bufSize) == 0) {
            f.get(); //throw out a ':'
            readThreshold(puckLimits, f);
        }
        else if(strncmp(buffer,cornerString.c_str() , bufSize) == 0) {
            f.get(); //throw out a ':'
            auto corners = Table::corners.getCorners();

            for(int i = 0; i < 4; i++) {
                cv::Point_<int> in;
                f >> in.x;
                f >> in.y;
                corners.emplace(corners.begin(), in);
            }

            if(corners.size() >= 4) { //we have what we need
                corners.resize(4);
                Table::corners.setCorners(corners); //update things
                Table::goals.recalculate(Table::corners);
                Table::setLimits();
            }

        }
        else {
            //this wasn't a valid setting?
            return false;
        }
    }
}

bool Settings::readConfigValues(const std::string& path) {
    //todo initialize to defaults
    if(path.empty()) {
        return false;
    }
    else {
        std::fstream f = std::fstream(path);
        if(!f.is_open()) {
            return false;
        }
        else {
            char buffer[256] = {};
            bool keepReading = true;
//            while(keepReading) {
//                keepReading = processLine(f, buffer, sizeof(buffer));
//            }

        }
    }
}

void Settings::readThreshold(threshold_s& t, fstream& f) {
    f << skipws;
    f >> t.minH;
    f >> t.maxH;
    f >> t.minS;
    f >> t.maxS;
    f >> t.minV;
    f >> t.maxV;
    f >> t.minArea;
    f >> t.maxArea;
    f.get(); //get the endl
}

void Settings::writeThreshold(const threshold_s& t, fstream& f) {
    f << t.minH << ' '
      << t.maxH << ' '
      << t.minS << ' '
      << t.maxS << ' '
      << t.minV << ' '
      << t.maxV << ' '
      << t.minArea << ' '
      << t.maxArea << ' '
      << endl;
}

bool Settings::writeConfigValues(const string& path) {
    std::fstream f;
    f.open("config.txt");
    char buffer[50];
    if (f.is_open()) {
        f << cameraUndistortString << delim << undistort << endl;
        f << malletLimitString << delim;
        writeThreshold(malletLimits, f);
        f << puckLimitString << delim
            << puckLimits.minH << ' '
            << puckLimits.maxH << ' '
            << puckLimits.minS << ' '
            << puckLimits.maxS << ' '
            << puckLimits.minV << ' '
            << puckLimits.maxV << ' '
            << puckLimits.minArea << ' '
            << puckLimits.maxArea << ' '
            << endl;
        auto corners = Table::corners.getCorners();
        f << cornerString << delim << corners[0] << ' ' << corners[1] << ' ' <<corners[2] << ' ' <<corners[3] << endl;

    }
    else {
        cout << "Failed to save settings" << endl;
        return false;
    }
    f.close();

}




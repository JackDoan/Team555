//
// Created by jes130330 on 3/8/2018.
//

#include "Config.h"
#include<iostream>
#include<fstream>


void Config::WriteToFile(){

    const std::string myfile = "C://AirHockeyRobot/js_calib_test.txt";
    std::fstream outputFile(myfile);
    outputFile.open(myfile, std::ios_base::out);

    if(outputFile.bad()){
        printf("file exists.\n");
    }

    //Printing coords to txt file ------ TODO: needs more editing, writing more than expected.
    if (outputFile.is_open()) {
            bool isEmpty(true);     //will use to check if there are already calib points in file
            std::string line;
            while(outputFile >> line)
                isEmpty = false;
            printf("Is file empty (before input given)?: %s\n",isEmpty ? "true" : "false");

            //tempCorners2 = getCalibratedCorners();

            outputFile.clear();
            outputFile << "writing" << std::endl;

//            for (int i = 0; i < 4; i++) {
//                //outputFile << "Corner " <<  i+1 << ": " << CalibratedCorners[i] << std::endl;
//                //outputFile << "Corner " << i+1 << ": " << corners[i] << std::endl;
//            }
            //for (int j = 0; j < 4; j++) {
             //   outputFile << "\nCalibrated Corner " <<  j+1 << ": " << CalibratedCorners[j];
            //}
            printf("File was opened\n");
            outputFile.close();
    } else printf("unable to open file.\n");
}
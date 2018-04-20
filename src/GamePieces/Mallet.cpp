//
// Created by mdl150330 on 3/14/2018.
//

#include "../../inc/GamePieces/Mallet.h"
#include "../../inc/Table.h"
#include <vector>
#include <math.h>
#include <thread>


Mallet::Mallet() {

    settingWindowName = const_cast<char *>("Mallet Settings 1");
    previewWindowName = const_cast<char *>("Mallet");

}

// TODO: add logic to mallet.findPuck that uses functions from camera, to adjust exposure and gain when mallet isn't found for a long time
// If speed is measured 'high' and mallet is not found, turn up exposure and gain until it is found again
// then once it is found again return it to its default setting





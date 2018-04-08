//
// Created by mdl150330 on 4/6/2018.
//

#include "Supervisor.h"

Supervisor::Supervisor() {

}

void Supervisor::run(bool idle) {
    if (idle) {
        // do the idle process

    } else {
        // decide what to do
    }
}

void Supervisor::decide() {
    // if puck is lost try to find it? (usually stuck under the belt and carrier of the robot)
    // if puck lost count is high (say > 1200) or if its lost for fewer like 300 frames and the last predicted
    // location is in the same band as the robot's current X position ie. did we predict it would end up under the belt

    // else if the puck is on our side of the table and moving slowly (ie all the predicted locations are 'clumped' up)
        // if the puck is outside of our "Strike box" we need to impulse it to get it unstuck. this is the only time
        // the robot should be allowed to exceed its "Strike box" position
        // if the puck is in our "Strike box" we need to call the attack procedure

    // else play defense
}

void Supervisor::idle() {
    // the idle process should show a series of 'slides' that provide educational information about engineering
}

void Supervisor::defense() {
    // call the offense procedure
}

void Supervisor::offense() {
    // call the offense procedure
}
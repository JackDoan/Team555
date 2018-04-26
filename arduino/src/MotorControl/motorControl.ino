#include <AccelStepper.h>
#include <MultiStepper.h>

#define X_STEP_PIN 2
#define X_DIR_PIN  3

#define Y_STEP_PIN 5
#define Y_DIR_PIN  6

typedef union longBytesU {
	struct {
		int lowest:8;
		int low:8;
		int high:8;
		int highest:8;
	} b;
	long l;
} longbytes;

void processCMD(char cmd, longbytes in);

// Define some steppers and the pins the will use
AccelStepper stepperX(AccelStepper::DRIVER, X_STEP_PIN, X_DIR_PIN);
AccelStepper stepperY(AccelStepper::DRIVER, Y_STEP_PIN, Y_DIR_PIN);

#define MICROSTEP_RATIO 1 

#define HOME_ACCEL 500.0 * MICROSTEP_RATIO
#define X_ACCEL 8000.0 * MICROSTEP_RATIO 
#define Y_ACCEL 50000.0 * MICROSTEP_RATIO


void setup() {
	// put your setup code here, to run once:
	stepperX.setMaxSpeed(X_ACCEL * MICROSTEP_RATIO);
	stepperX.setAcceleration(X_ACCEL);
	stepperY.setMaxSpeed(Y_ACCEL* MICROSTEP_RATIO); //200000
	stepperY.setAcceleration(Y_ACCEL); //30000
	Serial.begin(115200);
	pinMode(10,OUTPUT);
	digitalWrite(10, 1);
}

void loop() {
	if (Serial.available() >= 6) {
		// get the new byte:
		char inChar = (char) Serial.read();
		if (inChar == 0x69) { ///if the SOF is valid:
			char cmd = (char) Serial.read();
            longbytes rx;
			rx.b.highest = Serial.read();
			rx.b.high = Serial.read();
			rx.b.low = Serial.read();
			rx.b.lowest = Serial.read();
            processCMD(cmd, rx);
		}
	}
//	else { ///otherwise, flush the serial buffer?
//		while (Serial.available() > 0) {
//			char t = Serial.read();
//		}
//	}
	stepperX.run();
	stepperY.run();
}

void processCMD(char cmd, longbytes rx) {
    longbytes stepCount;
    switch (cmd) {
        case '!':
            stepperX.stop();
            stepperY.stop();
            break;
        case 'x':
	    stepperX.setAcceleration(HOME_ACCEL);
            stepperX.move(rx.l* MICROSTEP_RATIO);
            break;
        case 'y':
	    stepperY.setAcceleration(HOME_ACCEL);
            stepperY.move(rx.l* MICROSTEP_RATIO);
            break;
        case 'X':
	    stepperX.setAcceleration(X_ACCEL);
            stepperX.moveTo(rx.l* MICROSTEP_RATIO);
            break;
        case 'Y':
	    stepperY.setAcceleration(Y_ACCEL);
            stepperY.moveTo(rx.l* MICROSTEP_RATIO);
            break;
        case 'H':
            stepperX.setCurrentPosition(0);
            stepperY.setCurrentPosition(0);
            break;
        case 'O':
            stepperX.setMaxSpeed((float) rx.l* MICROSTEP_RATIO);
            break;
        case '{':
            stepperY.setMaxSpeed((float) rx.l* MICROSTEP_RATIO);
            break;
        case 'Q':
            rx.l = stepperX.distanceToGo() / MICROSTEP_RATIO;
            break;
        case 'W':
            rx.l = stepperY.distanceToGo() / MICROSTEP_RATIO;
            break;
        case 'f':
		digitalWrite(10, !digitalRead(10));
		break;
	case '?':
            if (rx.b.lowest == 'y') {
                stepCount.l = stepperY.currentPosition()/ MICROSTEP_RATIO;
            } else {
                stepCount.l = stepperX.currentPosition()/ MICROSTEP_RATIO;
            }
            rx = stepCount;
            Serial.write("i?");
            Serial.write(rx.b.highest);
            Serial.write(rx.b.high);
            Serial.write(rx.b.low);
            Serial.write(rx.b.lowest);
            break;
    }
}

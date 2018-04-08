//
// Created by mdl150330 on 3/6/2018.
//


#include "../inc/Serial.h"
#ifdef WINDOWS
Serial::Serial(const char *portName)  {

    this->connected = false; //We're not yet connected
    //Try to connect to the given port through CreateFile
    this->hSerial = CreateFile(portName,
                               GENERIC_READ | GENERIC_WRITE,
                               0,
                               NULL,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL, //FILE_FLAG_OVERLAPPED
                               NULL);

    //Check if the connection was successful
    if(this->hSerial==INVALID_HANDLE_VALUE) {
        //If not success full display an Error
        if(GetLastError()==ERROR_FILE_NOT_FOUND){
            printf("ERROR: Handle was not attached. Reason: %s not available.\n", portName);
        }
        else {
            printf("Serial port ERROR!!!");
        }
    }
    else { //If connected we try to set the comm parameters
        DCB dcbSerialParams = {0};

        //Try to get the current
        if (!GetCommState(this->hSerial, &dcbSerialParams)) {
            printf("failed to get current serial parameters!\n");
        }
        else {
            //Define serial connection parameters for the arduino board
            dcbSerialParams.BaudRate=CBR_115200;
            dcbSerialParams.ByteSize=8;
            dcbSerialParams.StopBits=ONESTOPBIT;
            dcbSerialParams.Parity=NOPARITY;
            //Setting the DTR to Control_Enable ensures that the Arduino is properly
            //reset upon establishing a connection
            dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

            //Set the parameters and check for their proper application
            if(!SetCommState(hSerial, &dcbSerialParams)) {
                printf("ALERT: Could not set Serial Port parameters\n");
            }
            else {
                /*printf("timeouts got set!\n");
                COMMTIMEOUTS timeouts={0};
                timeouts.ReadIntervalTimeout=10;
                timeouts.ReadTotalTimeoutConstant=10;
                timeouts.ReadTotalTimeoutMultiplier=1;
                timeouts.WriteTotalTimeoutConstant=10;
                timeouts.WriteTotalTimeoutMultiplier=1;
                if(!SetCommTimeouts(hSerial, &timeouts)){
                   printf("ALERT: Failed to set timeouts parameters\n");
                }*/
                //If everything went fine we're connected
                this->connected = true;
                //Flush any remaining characters in the buffers
                PurgeComm(this->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
                //We wait 2s as the arduino board will be resetting
                Sleep(ARDUINO_WAIT_TIME); //todo explain this to Jack
                // TODO: explain this to mike, because mike didn't put it here
            }
        }

    }

}

// -Mike : too dumb to fix this, fuck UTD they don't teach us enough programming
// -Jack : no worries bro. You had it set to the default destructor in the header file. Ez-pz fix
Serial::~Serial() {
    //Check if we are connected before trying to disconnect
    if(this->connected) {
        //We're no longer connected
        this->connected = false;
        //Close the serial handler
        CloseHandle(this->hSerial);
    }
}

int Serial::Available() {
    return this->status.cbInQue;
}

int Serial::ReadData(char *buffer, unsigned int nbChar) {
    //Number of bytes we'll have read
    DWORD bytesRead;
    //Number of bytes we'll really ask to read
    unsigned int toRead;

    //Use the ClearCommError function to get status info on the Serial port
    ClearCommError(this->hSerial, &this->errors, &this->status);

    //Check if there is something to read
    if(this->status.cbInQue>0) {
        //If there is we check if there is enough data to read the required number
        //of characters, if not we'll read only the available characters to prevent
        //locking of the application.
        if(this->status.cbInQue>nbChar) {
            toRead = nbChar;
        }
        else {
            toRead = this->status.cbInQue;
        }

        //Try to read the require number of chars, and return the number of read bytes on success
        if(ReadFile(this->hSerial, buffer, toRead, &bytesRead, nullptr) ) {
            return (int)bytesRead;
        }
        else {
            printf("Problem reading from serial port\n");
        }
    }
    //If nothing has been read, or that an error was detected return -1
    return -1;

}


bool Serial::WriteData(const char *buffer, unsigned int nbChar) {
    DWORD bytesSend;

    //Try to write the buffer on the Serial port
    if(!WriteFile(this->hSerial, (void *)buffer, nbChar, &bytesSend, nullptr)) {
        //In case it don't work get comm error and return false
        ClearCommError(this->hSerial, &this->errors, &this->status);
        return false;
    }
    else
        return true;
}

bool Serial::IsConnected() {
    //Simply return the connection status
    return this->connected;
}

#else

#include <fcntl.h>
#include <cerrno>
#include <cstdio>   /* Standard input/output definitions */
#include <cstring>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <termios.h> /* POSIX terminal control definitions */

//#define SERIALDEBUG 1




Serial::Serial(const char *portName)  {

    this->connected = false; //We're not yet connected
    //Try to connect to the given port through CreateFile

#ifdef SERIALDEBUG
    char file[] = "serial.log";
#else
    char file[] = "/dev/ttyACM0";
#endif


    this->hSerial =  open(file, O_RDWR | O_NOCTTY | O_NDELAY);
    if(this->hSerial == 0) {
        connected = false;
        printf("ERROR: Handle was not attached. errno %d\n", errno);
    }
    else {
        connected = true;
        struct termios options;
        tcgetattr(hSerial, &options); //Get the current options for the port
        cfmakeraw(&options);
        options.c_cflag |= (CLOCAL | CREAD); //Enable the receiver and set local mode ///pretty sure this isnt needed
        options.c_lflag &= ~(ECHO | ICANON | ISIG);
        cfsetispeed(&options, B115200); //Set the baud rates
        cfsetospeed(&options, B115200); //Set the baud rates
        tcsetattr(hSerial, TCSAFLUSH, &options); //set options, wait for operation to complete


    }


    //todo Check if the connection was successful
//    if(this->hSerial==INVALID_HANDLE_VALUE) {
//        //If not success full display an Error
//        if(GetLastError()==ERROR_FILE_NOT_FOUND){
//
//        }
//        else {
//            printf("Serial port ERROR!!!");
//        }
//    }

}


// -Mike : too dumb to fix this, fuck UTD they don't teach us enough programming
// -Jack : no worries bro. You had it set to the default destructor in the header file. Ez-pz fix
Serial::~Serial() {
    //Check if we are connected before trying to disconnect
    if(this->connected) {
        //We're no longer connected
        this->connected = false;
        //Close the serial handler
        close(this->hSerial);
    }
}

int Serial::Available() {
    return 0; //todo
    //return this->status.cbInQue;
}

int Serial::ReadData(char *buffer, unsigned int nbChar) {
    //Number of bytes we'll have read
    int bytesRead;
    //Number of bytes we'll really ask to read
    unsigned int toRead;
    //todo

    //Check if there is something to read
//    if(this->status.cbInQue>0) {
//        //If there is we check if there is enough data to read the required number
//        //of characters, if not we'll read only the available characters to prevent
//        //locking of the application.
//        if(this->status.cbInQue>nbChar) {
//            toRead = nbChar;
//        }
//        else {
//            toRead = this->status.cbInQue;
//        }
//
//        //Try to read the require number of chars, and return the number of read bytes on success
//        if(ReadFile(this->hSerial, buffer, toRead, &bytesRead, nullptr) ) {
//            return (int)bytesRead;
//        }
//        else {
//            printf("Problem reading from serial port\n");
//        }
//    }
    //If nothing has been read, or that an error was detected return -1
    return -1;

}


bool Serial::WriteData(const char *buffer, unsigned int nbChar) {
    int bytesSend;

    //Try to write the buffer on the Serial port
    if(this->hSerial == 0) {
        return false;
    }
#ifdef SERIALDEBUG
    auto result = fwrite(buffer, sizeof(char), nbChar, stdout );
    fflush(stdout);
#else
    auto result = write(hSerial, buffer, nbChar); //write(buffer, sizeof(char), nbChar, this->hSerial );
#endif
    //fflush(stdout);
    if(result != nbChar) {
        //In case it don't work get comm error and return false
        //todo get errno
        return false;
    }
    else
        return true;
}

bool Serial::IsConnected() {
    //Simply return the connection status
    return this->connected;
}

#endif
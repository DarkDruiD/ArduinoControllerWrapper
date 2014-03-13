#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>

#include <../USB_Host_Shield/Wii.h>
#include <../USB_Host_Shield/XBOXUSB.h>
#include <../USB_Host_Shield/PS3Enums.h>
#include <../USB_Host_Shield/controllerEnums.h>


class Controller {
    public:
        Controller (USB *usb);
        virtual bool connected () = 0;
        virtual double* getAngle () = 0;
        virtual bool* getButtons () = 0;
        virtual uint8_t* getHats () = 0;

        int getNumButts(){return numButts;}
        int getNumAngles(){return numAngs;}
        int getNumHats(){return numHats;}

        void setUp();

    protected:
        USB &usb;
        const static ButtonEnum buttons [];
        const static AnalogHatEnum hats [];
        const static AngleEnum angles [];

        static const int numButts;
        static const int numHats;
        static const int numAngs;
};


Controller::Controller(USB *usb) : usb(*usb){}


void Controller::setUp(){
    Serial.begin(115200);
    while(!Serial);

    if(usb.Init() == -1){
        Serial.print("Faildes to start USB");
    }
}

template <typename ps3Ctrl>
class ControllerPs3 : public Controller {
    public:
        ControllerPs3 (USB *usb, ps3Ctrl *ctrl);
        bool connected ();
        double* getAngle ();
        bool* getButtons ();
        uint8_t* getHats ();

    private:
        ps3Ctrl &ctrl;
        static const int numButts = 14;
        const static ButtonEnum buttons [numButts];

        static const int numHats = 4;
        const static AnalogHatEnum hats [numHats];

        static const int numAngs = 2;
        const static AngleEnum angles [numAngs];

};


template <typename ps3Ctrl>
const ButtonEnum ControllerPs3 <ps3Ctrl> ::buttons [numButts] =
    {UP, RIGHT, DOWN, LEFT, TRIANGLE, CIRCLE,
        CROSS, SQUARE, L2, R2, L1, R1, SELECT, START};


template <typename ps3Ctrl>
const AnalogHatEnum ControllerPs3 <ps3Ctrl> ::hats [numHats] =
    {LeftHatX, RightHatX, LeftHatX, LeftHatY};


template <typename ps3Ctrl>
const AngleEnum ControllerPs3 <ps3Ctrl> ::angles [numAngs] =
    {Pitch, Roll};


template <typename ps3Ctrl>
ControllerPs3 <ps3Ctrl> ::ControllerPs3 (USB *usb, ps3Ctrl *ctrl) : Controller (usb), ctrl (*ctrl) {}


template <typename ps3Ctrl>
bool ControllerPs3 <ps3Ctrl> ::connected (){
    usb.Task();
    return (ctrl.PS3Connected || ctrl.PS3NavigationConnected);
}


template <typename ps3Ctrl>
double* ControllerPs3 <ps3Ctrl> ::getAngle (){
    double *states = (double *) malloc(numAngs * sizeof(double));
    if(connected()){
        for(int index = 0; index < numButts; index++)
            states[index] = ctrl.getAngle(angles [index]);
    }
    return states;
}


template <typename ps3Ctrl>
bool* ControllerPs3 <ps3Ctrl> ::getButtons (){
    bool* states = (bool *) malloc(numButts * sizeof(bool));
    if(connected()){
        for(int index = 0; index < numButts; index++)
            states[index] = ctrl.getButtonClick(buttons [index]);
    }
    return states;
}


template <typename ps3Ctrl>
uint8_t* ControllerPs3 <ps3Ctrl> ::getHats (){
    uint8_t* states = (uint8_t *) malloc(numHats * sizeof(uint8_t));
    if(connected()){
        for(int index = 0; index < numHats; index++)
            states[index] = ctrl.getAnalogHat(hats [index]);
    }
    return states;
}

#endif
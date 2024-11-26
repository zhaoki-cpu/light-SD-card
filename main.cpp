#include "mbed.h"
#include "m3pi.h"
#include "APDS9960_I2C.h"
#include <stdlib.h>
#include <math.h>

m3pi m3pi;
APDS9960_I2C apds(p28,p27);

float calibration(){
    m3pi.stop();
    uint16_t cl = 0;
    float threshold = 0;
    for(int i = 0;i < 1000;i++){
        apds.readAmbientLight(cl);
        threshold+=cl;
        wait_ms(10);
    }
    threshold/=100;//get mean of ambient light
    
    return threshold*2;
}

// function returning random value 0~1
float rand_01() {
    int yi=0;
    yi = rand() %20;
    float yf=0;
    yf = (float)yi/20;
    return yf;
}



int main() {

    // Parameters that affect the performance
    float theta = 0.0, pre_theta = 0.0, rand_theta = 0.0;    
    float speed = 0.2;
    
    bool u = true; //input direction true:+ false:-
    float step = 2.0;
    float h = 0.0;

    
	uint16_t ambient_light = 0;
	uint16_t blue_light = 0, pre_blue_light = 0;

    // initialize sensor
    apds.init();
    apds.enablePower();
    apds.enableLightSensor();
    apds.setAmbientLightGain(3);
    wait(2);
    

    m3pi.locate(0,1);
    m3pi.printf("%d", blue_light);
    //m3pi.printf("Chemo Go");

    m3pi.sensor_auto_calibrate();

    wait(2.0);
    
    
    while (1) {
        // memorize previous data
        pre_blue_light = (uint16_t)blue_light;
        

        //get blue light
        blue_light = calibration();
        m3pi.printf("%d\n", blue_light);

        // judge input direction
        if ((uint16_t)blue_light >= pre_blue_light && h >= 0) {
            u = true;
        }
        else if ((uint16_t)blue_light >= pre_blue_light && h < 0) {
            u = false;
        }
        else if ((uint16_t)blue_light < pre_blue_light && h >= 0) {
            u = false;
        }
        else {
            u = true;
        }

        // move robot
        if (u) {
            m3pi.forward(speed);
            wait(step);
        }
        else {
            m3pi.left(0.5);
            wait(0.23);
            m3pi.forward(speed);
            wait(step);
        }

        // random rotate
        m3pi.right(0.5);
        rand_theta = rand_01();
        wait(1.0*rand_theta);
        pre_theta = (float)theta;
        theta = theta + rand_theta*2*3.14;
        if (!u) theta = theta - 3.14;
        m3pi.stop();
        wait(step);

        // calculate h
        if (u) {
            h = cos((float)pre_theta)*cos((float)theta) 
            + sin((float)pre_theta)*sin((float)theta);
        }
        else {            
            h = - cos((float)pre_theta)*cos((float)theta) 
            - sin((float)pre_theta)*sin((float)theta);
        }

        m3pi.cls();
    }
}

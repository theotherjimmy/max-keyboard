#include "mbed.h"
#include "rtos.h"
#include "USBKeyboard.h"

// Hardware serial port over DAPLink
Serial daplink(P2_1, P2_0);

DigitalOut rLED(LED1, LED_OFF);
DigitalOut gLED(LED2, LED_OFF);
DigitalOut bLED(LED3, LED_OFF);
DigitalIn button(P2_7, PullUp);
AnalogIn dipIOH(AIN_4);
AnalogIn swdIOH(AIN_5);
AnalogIn vddbIn(AIN_6);
AnalogIn vdd18In(AIN_7);
AnalogIn vdd12In(AIN_8);
AnalogIn vrtcIn(AIN_9);

USBKeyboard keyboard;

// Function to report failure
void testFailed()
{
    daplink.printf("\r\n! Test Failed !\r\n");
    rLED = LED_ON;
    gLED = LED_OFF;
    bLED = LED_OFF;
    while(1) {
        Thread::wait(500);
        gLED = !gLED;
    }
}

// main() runs in its own thread in the OS
// (note the calls to Thread::wait below for delays)
int main()
{
    float aIn;
    int ledCnt = 0;

    rLED = LED_ON;
    gLED = LED_ON;
    bLED = LED_OFF;

    daplink.printf("Checking Supplies\r\n");
    aIn = 2.4f * vdd12In;
    daplink.printf("vdd12 = %f\r\n", aIn);
    if ((aIn < 1.0f) || (aIn > 1.4f)) {
        testFailed();
    }
    aIn = 2.4f * vdd18In;
    daplink.printf("vdd18 = %f\r\n", aIn);
    if ((aIn < 1.6f) || (aIn > 2.0f)) {
        testFailed();
    }
    aIn = 4.8f * vddbIn;
    daplink.printf("vddb = %f\r\n", aIn);
    if ((aIn < 3.0f) || (aIn > 3.6f)) {
        testFailed();
    }
    aIn = 2.4f * vrtcIn;
    daplink.printf("vrtc = %f\r\n", aIn);
    if ((aIn < 1.6f) || (aIn > 2.0f)) {
        testFailed();
    }

    aIn = 6.0f * dipIOH;
    daplink.printf("dipIOH = %f\r\n", aIn);

    aIn = 6.0f * swdIOH;
    daplink.printf("swdIOH = %f\r\n", aIn);



    rLED = LED_OFF;
    Thread::wait(250);

    daplink.printf("Self Test Passed\r\n");
    rLED = LED_ON;
    gLED = LED_ON;
    bLED = LED_ON;

    while (true) {
        if (!button) {
            keyboard.mediaControl(KEY_MUTE);
            ledCnt++;
            gLED = ((ledCnt % 4) != 1);
            bLED = ((ledCnt % 4) != 2);
            rLED = ((ledCnt % 4) != 3);
            Thread::wait(500);
        } else {
            Thread::wait(50);
        }
    }
}


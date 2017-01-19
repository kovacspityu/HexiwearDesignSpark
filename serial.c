#include "mbed.h"
#include "rtos.h"

DigitalOut led_red(PTC8);
DigitalOut led_green(PTD0);
DigitalOut led_blue(PTC9);
Serial pc(USBTX, USBRX);

int main()
{
    pc.printf("Hello Design Spark\r\n");
    led_green=1;
    led_blue=1;
    while (true) {
        pc.printf("The RGB led is Red\r\n"); // print the situation of RGB LED
        led_red=0;
        Thread::wait(1000); // wait a second
        pc.printf("The RGB led is off\r\n"); // print the situation of RGB LED
        led_red=1;
        Thread::wait(1000); // wait a second
 
    }
}

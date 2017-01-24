#include "mbed.h"
#include "rtos.h"

#include "FXOS8700.h" // Accelerometer
#include "FXAS21002.h" // Gyroscope


// Pin connections
DigitalOut led_red(PTC8); // RGB LED - Red
DigitalOut led_green(PTD0); // RGB LED - Green
DigitalOut led_blue(PTC9); // RGB LED - Blue
Serial pc(USBTX, USBRX); // Serial interface

FXOS8700 accel(PTC11, PTC10); // Accelerometer
FXAS21002 gyro(PTC11,PTC10); // Gyroscope


// Variables
float accel_data[3]; // Storage for the data from the sensor
float accel_rms=0.0; // RMS value from the sensor
float ax, ay, az; // Integer value from the sensor to be displayed

float gyro_data[3];  // Storage for the data from the sensor
float gyro_rms=0.0; // RMS value from the sensor
float gx, gy, gz; // Integer value from the sensor to be displayed

int state = 0; // State of the situation
int faintAlarm = 0; // Alarm situation
int faintTest[10]; // faint test variables
int i;



int get_data()
{
    accel.acquire_accel_data_g(accel_data);
    gyro.acquire_gyro_data_dps(gyro_data);
    
    accel_rms = sqrt(((accel_data[0]*accel_data[0])+(accel_data[1]*accel_data[1])+(accel_data[2]*accel_data[2]))/3);
    gyro_rms = sqrt(((gyro_data[0]*gyro_data[0])+(gyro_data[1]*gyro_data[1])+(gyro_data[2]*gyro_data[2]))/3);
    
    if((accel_rms > 1 || gyro_rms>100) && faintAlarm==0)
    {
        state = 1; // Faint possible
        pc.printf("Faint detected 1");
    }
    else if (gyro_rms>5 & faintAlarm==1 )
    {
        state = 2; // Faint is an error go to normal condition
        pc.printf("Faint cancelled");
    }
    else
    {
        state = 0;
        pc.printf("Normal");
    }
    
    return state;
}

// Threads
void imu_thread()
{
    while(1)
    {
        get_data();
        Thread::wait(50); // wait a 50ms
    }
}





int main()
{
    accel.accel_config(); // Configure Accelerometer FXOS8700, Magnetometer FXOS8700
    
    gyro.gyro_config(); // Configure Gyroscope FXAS21002
    
    pc.printf("Hello Design Spark\r\n");
    led_green=1;
    led_blue=1;
    led_red=1;
    
    Thread getIMUData(imu_thread); // Start measuring data
    
    while (true) 
    {
        if(state)
        {  
           pc.printf("Faint test is started");

           getIMUData.terminate(); // Terminate measuring thread

           wait(5); // wait 5s
           faintAlarm = 1; // set alarm and check for motionless
           
           for( i=0; i<10; i++)
           {
               faintTest[i]=get_data();
                pc.printf("test %d \n",i);

               if(faintTest[i]==2) // motion detected faint alarm is not real
               {
                faintAlarm = 0;
                state = 3; // return to normal state
                i = 100; // exit loop 
                pc.printf("Faint alarm is cancelled");
                }
            }
            
            if(faintAlarm)
            {
                pc.printf("Faint alarm is started");
                led_red=0; // faint detected
            }
            
        }
        
        if(state ==3)
        {
           state = 0;
           pc.printf("Normal test started");
           Thread getIMUData(imu_thread); // Start measuring data in normal condition
        }
 
    }
}

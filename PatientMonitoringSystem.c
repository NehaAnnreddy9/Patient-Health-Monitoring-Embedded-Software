//Header files
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/utsname.h>
#include <stdlib.h>
#include <sched.h>
#include "../../BBBio_lib/BBBiolib.h"
#include <fcntl.h>
#include <string.h>
#include <float.h>
#include <stdbool.h>
#include <sys/types.h>

//Function declarations
void *myThreadFun1 (void *vargp);//Pulse sensor function
void *myThreadFun3 (void *vargp);//Temp sensor function
void *myThreadFun2 (void *vargp);//Motion sensor function
int setDirectionandValue(char gpiopin[]);//Function to set direction and value for GPIO ports
double CtoF(double c);//Function to convert celsius to fahrenheit 
double tempConv(char *string);//Function to convert analog signal value to temperature 
int setPriorities(pthread_attr_t tattr, struct sched_param param, int newprio);//Setting thread priorities to access Ftemp
void checkToSendMail();//Function to send mail to doctors in case of abnormal health conditions

//Defining ports
#define BUZZER "gpio20" 
#define RED_LED "gpio66"
#define GREEN_LED "gpio48"

pthread_attr_t tattr1, tattr2, tattr3;
pthread_t thread_id1, thread_id2, thread_id3;
double Ftemp;//Global Ftemp shared variable
int BPM = 0;

int main()
{
  int ret1, ret2, ret3;
  int newprio1 = 20, newprio2 = 21, newprio3 = 22;
  struct sched_param param1, param2, param3;

  //Setting priorities to threads
  ret1 = setPriorities(tattr1,param1,newprio1);
  ret2 = setPriorities(tattr2,param2,newprio2);
  ret3 = setPriorities(tattr3,param3,newprio3);
  
  printf("-------------Patient Monitoring System Started-----------------\n\n");
  /* with new priority specified */
  ret1 = pthread_create (&thread_id1, &tattr1, myThreadFun1, NULL);
  ret2 = pthread_create (&thread_id2, &tattr2, myThreadFun2, NULL);
  ret3 = pthread_create (&thread_id3, &tattr3, myThreadFun3, NULL);

  pthread_exit(NULL);
  return 0;
}

//Pulse sensor function - highest priority
void * myThreadFun1(void *vargp)
{
  float signalValue = 0;// Variable to store the value coming from the sensor
  int count = 0;
  bool started = false;//variable to show that pulse reading started
  int thres1 = 1510, thres2 = 1500;//thresholds for least and highest signal value
  int end_time, buzzertime;
  
  int red = setDirectionandValue(RED_LED);
  int buzzer = setDirectionandValue(BUZZER);
  int fd = open("/sys/bus/iio/devices/iio:device0/in_voltage3_raw", O_RDONLY);//AIN3 port for pulse sensor
  
  while(1){//continuous loop to check pulse
        end_time = time(NULL) + 30;//checking pulse for 30 secs
        buzzertime = time(NULL) + 2;//buzzer rings for 2 secs 
        while(time(NULL) < end_time) {
                char buffer[1024];
                int ret = read(fd, buffer, sizeof(buffer));
                signalValue = atoi(buffer);
               // printf("Pulse Signal value: %s\n", buffer);
                if (ret != -1) {
                        if (signalValue > thres1 && started == false)  
                        {
                                count++;
                                started = true;
                                write(red,"1",1);
                        }
                        else if (signalValue < thres2 && started == true)
                        {
                                started = false;
                                write(red,"0",1);
                        }
                        lseek(fd, 0, 0);
                }
                if(time(NULL) > buzzertime)
                {
                    write(buzzer,"0",1);
                }
                usleep(50000);
        }
        BPM = count*2;                            
        checkToSendMail();//Send mail to check for abnormal health conditions
        printf("PULSE RATE: CURRENT BPM = %d\n\n", BPM);
        if(BPM>100 || BPM<60)//buzzer rings when BPM>100 or BPM<60
        {
            write(buzzer,"1",1);
        }
        count = 0;
        started = false;
     }
        
  close(fd);
  return NULL;
}

//Temperature Sensor function - second highest priority
void * myThreadFun2(void *vargp)
{
  int fd = open("/sys/bus/iio/devices/iio:device0/in_voltage5_raw", O_RDONLY);//AIN5 port
 
        while (1) {//continuous loop for sensing
                char store[1024];
                int x = read(fd, store, sizeof(store));
                if (x != -1) {
                        store[x] = NULL;
                        double Ctemp = tempConv(store);//converting from analog values to celcius temp
                        Ftemp = CtoF(Ctemp);
                        printf("Temperature: Celsius: %f  Fahrenheit: %f\n", Ctemp, Ftemp);
                        lseek(fd, 0, 0);
                }
                sleep(1);
        }
        close(fd);
    
  return NULL;
}
 
//Motion sensor - least priority
void * myThreadFun3(void *vargp)
{
  int value;
  FILE * fp;
  char buffer[10];
  int green = setDirectionandValue(GREEN_LED);
  while(1){
    if ((fp = fopen("/sys/class/gpio/gpio67/value", "rb")) == NULL){//open GPIO 67 port for reading motion sensor values
            printf("Cannot open value file.\n");
            exit(1);
    }
    fread(buffer, sizeof(char), sizeof(buffer)-1, fp);
    value = atoi(buffer);
    if(value == 1)//checking for GPIO values 
    {
        printf("MOTION DETECTED!\n");
        write(green,"1",1);
    }
    else
    {
        write(green,"0",1);
    }
    sleep(1);
    fclose(fp);
  }
  return NULL;
}


//Function to set ports for LED and buzzer
 int setDirectionandValue(char gpiopin[])
 {
 	     int fd;
         char open1[40]="/sys/class/gpio/";
         char open2[40];
         strcat(open1,gpiopin);
         strcpy(open2, open1);
         strcat(open1,"/direction");
         fd = open(open1,O_RDWR);
         write(fd,"out",3);
         close(fd);
         strcat(open2,"/value");
         fd = open(open2,O_WRONLY);
         return fd;
}

//Function to send mail to doctors in case of abnormal health conditions
void checkToSendMail()
{
    if(Ftemp>100 || BPM<60 || BPM>100)
    {
        system("curl https://maker.ifttt.com/trigger/healthNotification/with/key/M5zUbVCz1diUeeDVek6Nz");
    }
}


// using the formula to convert to temprature
double tempConv(char *string) {
        int value = atoi(string);
        double mv = (value / 4096.0) * 1800;
        double temp = (mv - 500.0) / 10.0;
        return temp;
}


// setting the priority for the thread

int setPriorities(pthread_attr_t tattr, struct sched_param param, int newprio)
{
   int ret;
   ret = pthread_attr_init(&tattr);

/* safe to get existing scheduling param */
   ret = pthread_attr_getschedparam(&tattr, &param);

/* set the priority; others are unchanged */
   param.sched_priority = newprio;
   
/* setting the new scheduling param */
   ret = pthread_attr_setschedparam(&tattr, &param);
   
   return ret;
}

// converting the celius to fahrenheit
double CtoF(double c) {
        return (c * 9.0 / 5.0) + 32.0;
}
 
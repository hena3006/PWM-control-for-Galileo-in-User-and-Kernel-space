/* 
CSE 438 - Embedded Systems Programming
Assignment 2, Part 1
Team 15: Sharvil Shah (1213398171)
		 Hena Shah    (1213348511)
*/

//include the library files
#include <stdio.h>
#include <unistd.h>
#include <linux/input.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sched.h> 
#include <pthread.h>
#include <poll.h>

#define MOUSEFILE "/dev/input/event2"		//define the mouse event number as a macro
#define cycle_duration 20					//define cycle_duration for the program

int npin1[3],npin2[3],npin3[3];
static int flag=0;
int linuxio[14]= {11,12,13,14,6,0,1,38,40,4,10,5,15,7};
int levelshifter[14]={32,28,34,16,36,18,20,0,0,22,26,24,42,30};
int mux[14]={0,45,77,76,0,66,68,0,0,70,74,44,0,46};

int* checkpin(int pin, int *npin)
{
	npin[0]=linuxio[pin]; 
	npin[1]=levelshifter[pin];
	npin[2]=mux[pin];
	return npin;
}


void gp_export(int gpioport)				//self-explanatory function to export gpio pins
{
    FILE *fp = fopen("/sys/class/gpio/export","w");
    fprintf(fp,"%d",gpioport);
    fclose(fp);
}

void gp_unexport(int gpioport)				//self-explanatory function to unexport gpio pins
{
    FILE *fp = fopen("/sys/class/gpio/unexport","w");
    fprintf(fp,"%d",gpioport);
    fclose(fp);
}

void init_gpio(int gpioport)				//function to initialize gpio and set direction
{
    int Fd;
    char filename[256];
	sprintf(filename,"/sys/class/gpio/gpio%d/direction",gpioport);
	Fd = open(filename, O_WRONLY);
	if (Fd < 0)
	{
		//printf("\ngpio%d direction open failed",gpioport);
	}

	if(0> write(Fd,"out",3))
	{
		//("\nerror File\n");
	}
	close(Fd);
}

void setval(int gpioport,int val)			//self-explanatory function to setvalue
{
    int Fd;
    char filename[256];
 	sprintf(filename,"/sys/class/gpio/gpio%d/value",gpioport);

    Fd = open(filename,O_WRONLY);
    if (Fd < 0)
	{
		printf("\ngpio%d value open failed",gpioport);
	}
	if(val==0)
	{
		if(0> write(Fd,"0",1))
			printf("\nerror writing value for %d ",gpioport);
	}

	if(val==1)
	{
		if(0>write(Fd,"1",1))
			printf("\nerror writing value for %d ",gpioport );
	}

	close(Fd);
}

void* mouse(void* args)				//function that keeps track of a mouse click
{	
	//printf("\ninside mouse function\n");
	int fd;									
	struct input_event ie;
	fd = open(MOUSEFILE, 0);
	//printf("FD %d",fd);
	while(flag!= 1)
	{
		read(fd, &ie, sizeof(struct input_event));
		//printf("\nFD %d\n",fd);
		//printf("ie.code : %d\t ie.value : %d\n", ie.code, ie.value);		
		if((ie.code == 272 && ie.value == 1)||(ie.code == 273 && ie.value == 1))	
		{
			printf("\nClick detected");
			flag=1;
		}
	}
	close(fd);
	return 0;
}

int main()
{
	int Led1,Led2,Led3;		//general variale declarations
	int npin1[3],npin2[3],npin3[3];
	int on,off;
	int details[4];			//array to store the 3 pin numbers to which the led is connected, and the pwm value
	char filename[256];

	int i,j;
	
	//get the pin numbers and pwm value from the user and store them in the array details[] defined earlier
	printf("\nEnter pin number 1 between 0 to 13 : ");
    scanf("%d",&details[0]);
	if(details[0]<0 || details[0] >13)
	{
		printf("\nError in entering pin1, try again");
		goto exit_label;
	}
    checkpin(details[0],npin1);

	printf("\nEnter pin number 2 between 0 to 13 : ");
    scanf("%d",&details[1]);
	if(details[1]<0 || details[1]>13 || details[1] == details[0])
	{
		printf("\nError in entering pin2, try again");
		goto exit_label;
	}
    checkpin(details[1],npin2);

	printf("\nEnter pin number 3 between 0 to 13 : ");
    scanf("%d",&details[2]);
	if(details[2]<0 || details[2]>13 || details[2] == details[0] || details[2] == details[1])
	{
		printf("\nError in entering pin3, try again");
		goto exit_label;
	}
    checkpin(details[2],npin3);

	printf("\nEnter the Duty Cycle less than 100: ");
	scanf("%d",&details[3]);
	if(details[3]<1 || details[3]>100)
	{
		printf("\nError in entering duty cycle, try again");
		goto exit_label;
	}

	//the execution will not reach this part if the user enters an invalid pin on pwm value

	//export the pins
	gp_export(npin1[0]);
	for(j=1;j<3;j++)
	{
	 	if(npin1[j]!=0)
			gp_export(npin1[j]);
	}

	gp_export(npin2[0]);
	for(j=1;j<3;j++)
	{
	 	if(npin2[j]!=0)
			gp_export(npin2[j]);
	}

	gp_export(npin3[0]);
	for(j=1;j<3;j++)
	{
	 	if(npin3[j]!=0)
			gp_export(npin3[j]);
	}


	//call init_gpio function for initialization
   	init_gpio(npin1[0]);
	if(npin1[1]!=0)
		init_gpio(npin1[1]);
	if(npin1[2]<66||npin1[2]>79||npin1[2]!=0)
	{
	 
		init_gpio(npin1[2]);
	}

	init_gpio(npin2[0]);
	if(npin2[1]!=0)
		init_gpio(npin2[1]);
	if(npin2[2]<66||npin2[2]>79||npin2[2]!=0)
	{
	 
		init_gpio(npin2[2]);
	}

	init_gpio(npin3[0]);
	if(npin3[1]!=0)
		init_gpio(npin3[1]);
	if(npin3[2]<66||npin3[2]>79||npin3[2]!=0)
	{
	 
		init_gpio(npin3[2]);
	}

	//loops to setvalues
    for(j=1;j<3;j++)
	{
		if(npin1[j]!=0)
			setval(npin1[j],0);
	}
	
	for(j=1;j<3;j++)
	{
		if(npin2[j]!=0)
			setval(npin2[j],0);
	}

	for(j=1;j<3;j++)
	{
		if(npin3[j]!=0)
			setval(npin3[j],0);
	}	
	
	//compute the on and off times based on the value of pwm provided by the user
	on=(cycle_duration*1000*details[3])/100;
	off=(cycle_duration*1000)-on;		
	
	//create a thread which keeps on reading mouse. If a click is detected,
	//it will set flag value to 1.	

	pthread_t thread;	
	pthread_create (&thread, NULL, &mouse, NULL);

	sprintf(filename,"/sys/class/gpio/gpio%d/value",npin1[0]);
	Led1 = open(filename,O_WRONLY);
	sprintf(filename,"/sys/class/gpio/gpio%d/value",npin2[0]);
	Led2 = open(filename,O_WRONLY);
	sprintf(filename,"/sys/class/gpio/gpio%d/value",npin3[0]);
	Led3 = open(filename,O_WRONLY);

	/*main function to blink the leds 1 at a time, twice at a time and thrice at a time
	  note that after every set, the function checks the value of flag. */
	
	//If it is equal to 1 i.e a click has been detected, it will terminate the program
	
	/*it should be noted that the entire led blinking part of the main program can also be
	 implemented as a separate pthread. */
	while(flag!=1)
	{
		if(flag!=1)		
		{	

			for(i=0; i<25; i++){
			write(Led1,"1",1);
			usleep(on);
			write(Led1,"0",1);
			usleep(off);}
		}

		if(flag!=1)		
		{	
			for(i=0; i<25; i++){
			write(Led2,"1",1);
			usleep(on);
			write(Led2,"0",1);
			usleep(off);}
		}

		if(flag!=1)		
		{			
			for(i=0; i<25; i++){
			write(Led3,"1",1);
			usleep(on);
			write(Led3,"0",1);
			usleep(off);}	
		}

		if(flag!=1)		
		{			
			for(i=0; i<25; i++){
			write(Led1,"1",1);
			write(Led2,"1",1);
			usleep(on);
			write(Led1,"0",1);
			write(Led2,"0",1);
			usleep(off);}
		}

		if(flag!=1)		
		{	
			for(i=0; i<25; i++){
			write(Led2,"1",1);
			write(Led3,"1",1);
			usleep(on);
			write(Led2,"0",1);
			write(Led3,"0",1);
			usleep(off);}
		}
	
		if(flag!=1)		
		{
			for(i=0; i<25; i++){
			write(Led1,"1",1);
			write(Led3,"1",1);
			usleep(on);
			write(Led1,"0",1);
			write(Led3,"0",1);
			usleep(off);}
		}
	
		if(flag!=1)		
		{	
			for(i=0; i<25; i++){
			write(Led1,"1",1);
			write(Led2,"1",1);
			write(Led3,"1",1);
			usleep(on);
			write(Led1,"0",1);
			write(Led2,"0",1);
			write(Led3,"0",1);
			usleep(off);}
		}	
	}


	//close the Leds
	close(Led1);
	close(Led2);
	close(Led3);

	//use unexport function to unexport the pins
	gp_unexport(npin1[0]);
	for(j=1;j<3;j++)
	{
	 	if(npin1[j]!=0)
			gp_unexport(npin1[j]);
	}

	gp_unexport(npin2[0]);
	for(j=1;j<3;j++)
	{
	 	if(npin2[j]!=0)
			gp_unexport(npin2[j]);
	}

	gp_unexport(npin3[0]);
	for(j=1;j<3;j++)
	{
	 	if(npin3[j]!=0)
			gp_unexport(npin3[j]);
	}
	
	exit_label : return 0;

}
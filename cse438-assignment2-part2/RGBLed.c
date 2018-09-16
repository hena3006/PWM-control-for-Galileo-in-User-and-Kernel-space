#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/ioctl.h>
#include <sched.h>
#include <unistd.h>
#include <pthread.h>
#include <linux/kernel.h>
#include <sys/types.h>
#include <sys/stat.h>
#define CONFIG 1
#define DRIVER "/dev/RGBLed"
#define MOUSEFILE "/dev/input/event2"

int flag=0;
struct message							
{
	int gpio1;
	int gpio2;
	int gpio3;
    	int pwm;
}gptr;

void* mouse(void* args)				//function that keeps track of a mouse click
{	
	
	int fd;	
	printf("here");								
	struct input_event ie;
	fd = open(MOUSEFILE, 0);
	while(flag!= 1)
	{
		read(fd, &ie, sizeof(struct input_event));		
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
int fd1,fd2,k,q;
char str[3];
pthread_t thread;
//below code gets the gpio pins and dutycycle from the user	
pthread_create (&thread, NULL, &mouse, NULL);
printf("\nEnter pin number 1 between 0 to 13 : ");
    scanf("%d",&gptr.gpio1);
printf("\nEnter pin number 2 between 0 to 13 : ");
    scanf("%d",&gptr.gpio2);
printf("\nEnter pin number 3 between 0 to 13 : ");
    scanf("%d",&gptr.gpio3);
printf("\nEnter Duty cycle between 1 to 99 : ");
    scanf("%d",&gptr.pwm);
	
printf("first");
fd1 = open(DRIVER, O_RDWR);
if(fd1<0)
	printf("opening error");
fd2=ioctl(fd1,1,&gptr);
if(fd2<0)
	{
	printf("Invalid arguments try again");
	goto a1;
	}

//sequence for blinking the LED till mouse click is detected 
while(flag!=1)
{
	if(flag!=1)
	{
	q=1;
	printf("1");
	sprintf(str,"%d",q);
	
	k = write(fd1,(void *)str,3);
	if(k<0)
		printf("writing error");
	//usleep(500000);
	}
	
	
	if(flag!=1)
	{
	q=2;
	printf("2");
	sprintf(str,"%d",q);
	
	k = write(fd1,(void *)str,3);
	if(k<0)
		printf("writing error");
	//usleep(500000);
	}

	
	if(flag!=1)
	{
	q=4;
	printf("3");
	sprintf(str,"%d",q);
	
	k = write(fd1,(void *)str,3);
	if(k<0)
		printf("writing error");
	//usleep(500000);
	}
	
	
	if(flag!=1)
	{
	q=3;
	printf("4");
	sprintf(str,"%d",q);
	
	k = write(fd1,(void *)str,3);
	if(k<0)
		printf("writing error");
	//usleep(500000);
	}
	
	
	if(flag!=1)
	{
	q=5;
	printf("5");
	sprintf(str,"%d",q);
	k = write(fd1,(void *)str,3);
	if(k<0)
		printf("writing error");
	//usleep(500000);
	}
	
	
	if(flag!=1)
	{
	q=6;
	printf("6");
	sprintf(str,"%d",q);
	
	k = write(fd1,(void *)str,3);
	if(k<0)
		printf("writing error");
	//usleep(500000);
	}

	

	if(flag!=1)
	{
	q=7;
	printf("7");
	sprintf(str,"%d",q);
	k = write(fd1,(void *)str,3);
	if(k<0)
		printf("writing error");
	//usleep(500000);
	}

	
}
	q=0;
	printf("7");
	sprintf(str,"%d",q);
	k = write(fd1,(void *)str,3);
	if(k<0)
		printf("writing error");
	//usleep(500000);
	

a1:return 0;
}

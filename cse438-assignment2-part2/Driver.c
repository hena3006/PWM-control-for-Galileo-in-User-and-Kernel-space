/*------------------------------------
Hena Shah 
ASU ID 1213348511

Sharvil Shah
ASU ID 1213398171
--------------------------------------*/


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <asm/ioctl.h>
#include <linux/ioctl.h>
#include <linux/string.h>
#include <linux/device.h>
#include <linux/jiffies.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/timer.h>
#include <linux/gpio.h>
#include <linux/delay.h>
//#include <linux/gpio_lib.c>
//#include <linux/gpio.c>
#include <linux/proc_fs.h>
#include <linux/delay.h>
//static unsigned int gpioLED = 13;
#define DEVICE_NAME "MYDRIVER"
#define CONFIG 1
//#define MS_TO_NS(x) (x * 1E6L)

static struct timer_list my_timer;
struct class *class1;
static dev_t RGB_Led;
#define cycle_duration 20
int linuxio[14]= {11,12,13,14,6,0,1,38,40,4,10,5,15,7};		//arrays used for dynamically assigning gpio pins
int levelshifter[14]={32,28,34,16,36,18,20,100,100,22,26,24,42,30};
int mux[14]={100,45,77,76,100,66,68,100,100,70,74,44,100,46};
int pin1[3];
int pin2[3];
int pin3[3];
int on;
int off;
int red;
int blue;
int green;
int red1;
int blue1;
int green1;
struct message			//structure of input received from user						
{
	int gpio1;
	int gpio2;
	int gpio3;
    	int pwm;
};



static struct dev		//device structure
{
	struct cdev char_device;
	struct message q1;
	char *name;
	int q;
}* my_device;



static void my_timer_function(unsigned long data)	//timer function of led on and led off depending 
{
	
	//printk( KERN_NOTICE "I was here, jiffies=%lu\n", jiffies );

	if(!(red1==0&&green1==0&&blue1==0))
	{
		red1=0;
		green1=0;
		blue1=0;
		gpio_set_value(pin1[0],red1);
		gpio_set_value(pin2[0],green1);
		gpio_set_value(pin3[0],blue1);
		my_timer.expires += (off*HZ)/10;
		add_timer(&my_timer);
	}
	
	else
	{
		red1=red;
		green1=green;
		blue1=blue;
		gpio_set_value(pin1[0],red1);
		gpio_set_value(pin2[0],green1);
		gpio_set_value(pin3[0],blue1);		
		my_timer.expires += (on*HZ)/10;
		add_timer(&my_timer);
	}	
	
	return;	
}


int my_open(struct inode *i, struct file *f)
{
	struct dev *devp;
	devp = container_of(i->i_cdev, struct dev, char_device);
	f->private_data = devp;
	printk(KERN_INFO"\n%s is opening\n", devp->name);
	return 0;
}

int my_close(struct inode *i, struct file *f)
{
	struct dev *devp=f->private_data;
	printk(KERN_INFO"\n%s is closing\n", devp->name);
	return 0;
}

ssize_t sq_write(struct file *f,const char *buff, size_t count, loff_t *offp) //getting data from the user space and blinking the leds for .5 sec in the kernel space
{	

	int a;
	struct dev *my_device = f->private_data;
	get_user(my_device->q,buff);
	a=my_device->q-48;
	red=(a & 0x04);
	green=(a & 0x02);
	blue=(a & 0x01);
	if(red==4)
	{
		red=1;
		gpio_set_value(pin1[0], red);
	}
	if(green==2)
	{
		green=1;
	}
	if(blue==1)
	{
		blue=1;
	}
	
	red1=red;
	green1=green;
	blue1=blue;
	gpio_set_value(pin1[0],red1);
	gpio_set_value(pin2[0],green1);
	gpio_set_value(pin3[0],blue1);	
	
	my_timer.function = my_timer_function;		
	my_timer.expires += (on*HZ);
	my_timer.data = 0;
	add_timer(&my_timer);
	msleep(500);	//we are using msleep here not in the user space bacause the kernel is hanging and we are getting segmentation fault
	del_timer(&my_timer);
	return 0;
}

static long ioctl_funcs(struct file *f,unsigned int cmd, unsigned long count)  //ioctl function for setting the pins aswell as MUX pins and level shifter
//we are intializing the timer here
{
		
	int i,ret;
	struct message *gptr;
	printk("here");
	gptr = (struct message*)kmalloc(sizeof(struct message),GFP_KERNEL);	
	if(copy_from_user(gptr,(struct message *)count,sizeof(struct message)))
		printk("Can't write in %s\n", my_device->name);  
	printk(" %d",gptr->gpio1);
	printk(" %d",gptr->gpio2);
	printk(" %d",gptr->gpio3);
	printk(" %d",gptr->pwm);
	if(gptr->gpio1<0||gptr->gpio1>13||gptr->gpio1==7||gptr->gpio1==8)
		return -1;
	if(gptr->gpio2<0||gptr->gpio2>13||gptr->gpio2==gptr->gpio1||gptr->gpio2==7||gptr->gpio2==8)
		return -1;
	if(gptr->gpio3<0||gptr->gpio3>13||gptr->gpio3==gptr->gpio1||gptr->gpio3==gptr->gpio2||gptr->gpio3==7||gptr->gpio3==8)
		return -1;
	if(gptr->pwm<1||gptr->pwm>99)
		return -1;
	pin1[0]=linuxio[gptr->gpio1]; 
	pin1[1]=levelshifter[gptr->gpio1];
	pin1[2]=mux[gptr->gpio1];
	pin2[0]=linuxio[gptr->gpio2]; 
	pin2[1]=levelshifter[gptr->gpio2];
	pin2[2]=mux[gptr->gpio2];
        pin3[0]=linuxio[gptr->gpio3]; 
	pin3[1]=levelshifter[gptr->gpio3];
	pin3[2]=mux[gptr->gpio3];
	on=(cycle_duration*gptr->pwm)/100;
	off=(cycle_duration)-on;
	//printk("%d",on);
	//printk("%d",off);
	gpio_request(pin1[0], "RED");
	//printk("%d",ret);
 	gpio_request(pin2[0],"BLUE");
	gpio_request(pin3[0],"GREEN");
	for(i=0;i<3;i++)
	{
		if(pin1[i]!=100)
		{

			gpio_export(pin1[i],false);
			
		}
	}
	for(i=0;i<3;i++)
	{
		if(pin2[i]!=100)
		{
			gpio_export(pin2[i],false);
			
		}
	}
	for(i=0;i<3;i++)
	{
		if(pin3[i]!=100)
		{
			gpio_export(pin3[i],false);
		}
	}
	ret=gpio_direction_output(pin1[0],1);
	if(ret<0)
		printk("direction error");
	ret=gpio_direction_output(pin2[0],1);
	if(ret<0)
		printk("direction error");
	ret=gpio_direction_output(pin3[0],1);
	if(ret<0)
		printk("direction error");
	if(pin1[1]!=100)
		gpio_direction_output(pin1[1],0);
	if(pin1[2]<66||pin1[2]>79)
	{
	 	if(pin1[2]!=100)
			ret=gpio_direction_output(pin1[2],0);
			if(ret<0)
				printk("direction error");	
	}
	if(pin2[1]!=100)
		ret=gpio_direction_output(pin2[1],0);
		if(ret<0)
			printk("direction error");
	if(pin2[2]<66||pin2[2]>79)
	{
	 	if(pin2[2]!=100)
			gpio_direction_output(pin2[2],0);
			if(ret<0)
				printk("direction error");
	}
	if(pin3[1]!=100)
		ret=gpio_direction_output(pin3[1],0);
		if(ret<0)
			printk("direction error");
	if(pin3[2]<66||pin3[2]>79)
	{
	 	if(pin3[2]!=100)
			gpio_direction_output(pin3[2],0);
			if(ret<0)
				printk("direction error");
	}
	
	init_timer(&my_timer);		//initializing the timer
	return 0;
}
    
     
// File operations structure.
static const struct file_operations my_operations = {
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_close,
	.unlocked_ioctl	= ioctl_funcs,
	.write = sq_write,
	
};


int __init mydriver_init(void)  //Driver Initialization
{
	int err;
	printk(KERN_ALERT "initializing\n");
	
	 //Request dynamic allocation of a device major number 
	if(alloc_chrdev_region(&RGB_Led,0,1,DEVICE_NAME)<0)
	{
		printk(KERN_DEBUG "Can't register device\n");
	}
	if((class1=class_create(THIS_MODULE,DEVICE_NAME)) == NULL)
	{
		unregister_chrdev_region(RGB_Led, 1);
	}

		my_device = (struct dev*) kmalloc(sizeof(struct dev), GFP_KERNEL);
		if (!my_device)
		{
			printk("Bad kmalloc\n");
			return -ENOMEM;
		}
		
		
	printk("2\n");
	my_device->name="RGBLed";
	
	
	
		if(device_create(class1, NULL, MKDEV(MAJOR(RGB_Led), MINOR(RGB_Led)), NULL, "RGBLed") == NULL)
		{
			class_destroy(class1);
			unregister_chrdev_region(RGB_Led, 1);
			printk(KERN_DEBUG "Can't register input device\n");
			return -1;
		}
	
		//Connect the file operations with the cdev
		cdev_init(&(my_device->char_device), &my_operations);
		my_device->char_device.owner = THIS_MODULE;
		err=cdev_add(&my_device->char_device,MKDEV(MAJOR(RGB_Led), MINOR(RGB_Led)), 1);
		if(err)
		{
			printk("Bad cdev\n");
			return err;
		}
	
	printk(KERN_INFO" created as device");
	
 	
 
	return 0;
}

void __exit mydriver_exit(void)
{
	int i;
	for(i=0;i<3;i++)
	{
		if(pin1[i]!=100)
		{

			gpio_unexport(pin1[i]);
			
		}
	}
	for(i=0;i<3;i++)
	{
		if(pin2[i]!=100)
		{
			gpio_unexport(pin2[i]);
			
		}
	}
	for(i=0;i<3;i++)
	{
		if(pin3[i]!=100)
		{
			gpio_unexport(pin3[i]);
		}
	}
	// device_remove_file(gmem_dev_device, &dev_attr_xxx);
	// Release the major number 
	cdev_del(&my_device->char_device);
	device_destroy(class1, MKDEV(MAJOR(RGB_Led), MINOR(RGB_Led)));
	kfree(my_device);
	//Destroy driver_class 
	class_destroy(class1);
	unregister_chrdev_region(RGB_Led,0);
	printk(KERN_INFO" deleted as device");
}

module_init(mydriver_init);
module_exit(mydriver_exit);
MODULE_LICENSE("GPL v2");

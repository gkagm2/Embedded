#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <ioctl.h>

//Drivers define
//1. LCD Driver Setting
#define MAX_BUFF 32
#define LINE_BUFF 16
#define FPGA_TEXT_LCD_DEVICE "/nfsroot/fpga_text_lcd"

//Lottery value
#define LOTTERY_NO_COUNT 45
#define LOTTERY_DIGIT 6

//function define


int main(int argc, char **argv) {

	int i, dev, str_size, chk_size;
	unsigned char string[32];

	memset(string,0,sizeof(string));
	if(argc<2&&argc>3) {
	printf("Invalid Value Arguments!\n");
	return -1;
	
	if(strlen(argv[1])>LINE_BUFF||strlen(argv[2])>LINE_BUFF){
		printf("16 alphanumeric characters on a line!\n");
		return -1;
	}
	dev = open(FPGA_TEXT_LCD_DEVICE, O_WRONLY);
	if (dev<0) {
		printf("Device open error : %s\n",FPGA_TEXT_LCD_DEVICE);
		return -1;
	}
	str_size=strlen(argv[1]);
	if(str_size>0) {
		strncat(string,argv[1],str_size);
		memset(string+str_size,' ',LINE_BUFF-str_size);
	}
	str_size=strlen(argv[2]);
	if(str_size>0) {
		strncat(string,argv[2],str_size);
		memset(string+LINE_BUFF+str_size,' ',LINE_BUFF-str_size);
	}
	write(dev,string,MAX_BUFF);
	close(dev);
	return(0);
}
		




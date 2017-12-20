#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <time.h>

// dot_matrix header
#include "./fpga_dot_font.h"

// 드라이버 define
#define FPGA_TEXT_LCD_DEVICE "/dev/fpga_text_lcd"
#define FPGA_PUSH_DEVICE "/dev/fpga_push_switch"
#define FPGA_DOT_DEVICE "/dev/fpga_dot"
#define FPGA_BUZZER_DEVICE "/dev/fpga_buzzer"
#define FPGA_FND_DEVICE "/dev/fpga_fnd"
#define FPGA_LED_DEVICE "/dev/fpga_led"
#define FPGA_STEP_MOTOR_DEVICE "/dev/fpga_step_motor"

// driver 갯수
#define MAX_DEVICE 7

// text lcd define
#define MAX_BUFF 32
#define LINE_BUFF 16
#define INTRO_STRING1 "Start movie" 
#define INTRO_STRING2 "reserve service"

// push_switch define
#define MAX_BUTTON 9

// step_motor define
#define MOTOR_ATTRIBUTE_ERROR_RANGE 4

// fnd define
#define MAX_DIGIT 4

void * timeout(void * arg); 
pthread_mutex_t mutex;
int mtime = 0;

int dev[MAX_DEVICE]; 
// 0번 : text_lcd 
// 1번 : push_switch 
// 2번 : dot_matrix 
// 3번 : led
// 4번 : step_motor
// 5번 : fnd
// 6번 : buzzer

// led 전역 변수
unsigned char led_data;
int exit_flag = 0;

int main()
{
	pthread_t timeout_t;
	int i, j;
	
	// text_lcd var
    int lcd_str_size;
    int lcd_chk_size;
	int movie_choice;
	char movie_list[4][17];
	char menual[17];
	char cur_seat[17];
	char col[2];
	unsigned char lcd_string[32];
	
	// push_switch var
	int push_buff_size;
	unsigned char push_sw_buff[MAX_BUTTON];
	
	// dot_matrix var
	int dot_str_size;
	int dot_set_num;
	int dot_location;
	int dot_flag;
	int dot_check;
	int dot_check2;
	
	// led var
	unsigned char led_retval;
	
	// step_motor var
	unsigned char motor_state[3];
	
	// fnd var
	int fnd_cnt;
	unsigned char fnd_data[4];
	
	// buzzer var
	unsigned char bzr_data;
	
	srand(time(NULL));

	memset(lcd_string,0,sizeof(lcd_string)); // text_lcd
	memset(motor_state,0,sizeof(motor_state)); // step_motor
	memset(fnd_data,0,sizeof(fnd_data)); // fnd
	
	dev[0] = open(FPGA_TEXT_LCD_DEVICE, O_WRONLY);
	dev[1] = open(FPGA_PUSH_DEVICE, O_RDWR);
	dev[2] = open(FPGA_DOT_DEVICE, O_WRONLY);
	dev[3] = open(FPGA_LED_DEVICE, O_RDWR);
	dev[4] = open(FPGA_STEP_MOTOR_DEVICE, O_WRONLY);
	dev[5] = open(FPGA_FND_DEVICE, O_RDWR);
	dev[6] = open(FPGA_BUZZER_DEVICE, O_RDWR);
	
	for(i=0;i<MAX_DEVICE;i++) {
		if (dev[i]<0) {
			printf("Device open error : %d\n", i);
			return -1;
		}
	}
	printf("device open success\n");
	
	led_data = 0;
	write(dev[3], &led_data, 1);
	write(dev[2],fpga_set_blank,sizeof(fpga_set_blank)); // dot Matrix init
	// fnd init
	for (i=0;i<4;i++)
		fnd_data[i] = (unsigned char)0;
	write(dev[5], &fnd_data, 4);
	
	// intro
	lcd_str_size = strlen(INTRO_STRING1);
	strncat(lcd_string, INTRO_STRING1, lcd_str_size);
	memset(lcd_string + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);
	
	lcd_str_size = strlen(INTRO_STRING2);
	strncat(lcd_string, INTRO_STRING2, lcd_str_size);
	memset(lcd_string + LINE_BUFF + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);
	
	write(dev[0], lcd_string, MAX_BUFF);
	
	usleep(3000000);
	memset(lcd_string, 0, sizeof(lcd_string));
	
	// movie choice
	strcpy(movie_list[0], "Frozen, 2013");
	strcpy(movie_list[1], "Hunger Games");
	strcpy(movie_list[2], "Spectre, 2015");
	strcpy(movie_list[3], "Batman");
	strcpy(menual, "4 up 5 slt 6 dwn");
	
	lcd_str_size = strlen(movie_list[0]);
	strncat(lcd_string, movie_list[0], lcd_str_size);
	memset(lcd_string + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);
	
	lcd_str_size = strlen(menual);
	strncat(lcd_string, menual, lcd_str_size);
	memset(lcd_string + LINE_BUFF + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);
				
	write(dev[0], lcd_string, MAX_BUFF);
	led_data = 128;
	write(dev[3], &led_data, 1);
	
	pthread_create(&timeout_t, NULL, timeout, NULL);
	pthread_mutex_init(&mutex, NULL);
	
	movie_choice = 0; // cur movie
	push_buff_size=sizeof(push_sw_buff); // push_sw_buff size
	while(1) {
		if (exit_flag == 1) {
		
			lcd_str_size = strlen("Time Out!!!");
			strncat(lcd_string, "Time Out!!!", lcd_str_size);
			memset(lcd_string + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);
	
			lcd_str_size = strlen("");
			strncat(lcd_string, "", lcd_str_size);
			memset(lcd_string + LINE_BUFF + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);
				
			write(dev[0], lcd_string, MAX_BUFF);
		
			return 0;
		}
		usleep(300000);
		read(dev[1], &push_sw_buff, push_buff_size);
		memset(lcd_string, 0, sizeof(lcd_string));
		if (push_sw_buff[3] && movie_choice >= 1) {
			movie_choice--;
			led_data *= 2;
			
			pthread_mutex_lock(&mutex); // mutex lock	
			write(dev[3], &led_data, 1);
			pthread_mutex_unlock(&mutex); // mutex unlock
			
			lcd_str_size = strlen(movie_list[movie_choice]);
			strncat(lcd_string, movie_list[movie_choice], lcd_str_size);
			memset(lcd_string + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);
		
			write(dev[0], lcd_string, LINE_BUFF);
		
			mtime = 0;
		}
		else if (push_sw_buff[5] && movie_choice <= 2) {
			movie_choice++;
			led_data /= 2;
			
			pthread_mutex_lock(&mutex); // mutex lock
			write(dev[3], &led_data, 1);
			pthread_mutex_unlock(&mutex); // mutex unlock
			
			lcd_str_size = strlen(movie_list[movie_choice]);
			strncat(lcd_string, movie_list[movie_choice], lcd_str_size);
			memset(lcd_string + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);
		
			write(dev[0], lcd_string, LINE_BUFF);
		
			mtime = 0;
		}
		else if (push_sw_buff[4]) {
			read(dev[0], lcd_string, MAX_BUFF);
			pthread_mutex_lock(&mutex); // mutex lock
			write(dev[3], &led_data, 1);
			pthread_mutex_unlock(&mutex); // mutex unlock
			mtime = 0;
			break;
		}
	}
	
	printf("%s\n", lcd_string);
	
	// dot Matrix 초기화
	write(dev[2],fpga_set_full,sizeof(fpga_set_full));
	usleep(50000);
	write(dev[2],fpga_set_blank,sizeof(fpga_set_blank));
	usleep(50000);
	
	// dot Matrix random value input
	for (i=0;i<5;i++) {
		fpga_on[i+5] = fpga_off[i+5] = fpga_data[i+5] = rand()%128;
	}
	
	dot_flag = 7;
	dot_location = 5;
	fpga_on[dot_location] += 0x40;
	write(dev[2], fpga_on, sizeof(fpga_on));
	
	while(1) {
		dot_check = 1;
		
		if (exit_flag == 1) {
			return 0;
		}
			
		switch(dot_location) {
		case 5:
			strcpy(cur_seat, "A-");
			break;
		case 6:
			strcpy(cur_seat, "B-");
			break;
		case 7:
			strcpy(cur_seat, "C-");
			break;
		case 8:
			strcpy(cur_seat, "D-");
			break;
		case 9:
			strcpy(cur_seat, "E-");
			break;
		}
		
		sprintf(col, "%d", (8 - dot_flag));
		strcat(cur_seat, col);
		
		memset(lcd_string, 0, sizeof(lcd_string));
		lcd_str_size = strlen(movie_list[movie_choice]);
		strncat(lcd_string, movie_list[movie_choice], lcd_str_size);
		memset(lcd_string + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);
		
		lcd_str_size = strlen(cur_seat);
		strncat(lcd_string, cur_seat, lcd_str_size);
		memset(lcd_string + LINE_BUFF + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);
		write(dev[0], lcd_string, MAX_BUFF);
		
		usleep(150000);
		write(dev[2], fpga_off, sizeof(fpga_off));
		
		usleep(150000);
		write(dev[2], fpga_on, sizeof(fpga_on));
		
		read(dev[1], &push_sw_buff, push_buff_size);
		if (push_sw_buff[1] && dot_location > 5) {
			fpga_on[dot_location] = fpga_data[dot_location];
			fpga_off[dot_location] = fpga_data[dot_location]; 
			
			dot_check2 = fpga_on[dot_location-1];
			for (i=0;i<dot_flag-1;i++) {
				dot_check *= 2;
				dot_check2 /= 2;
			}
			dot_check2 %= 2;
			
			if (dot_check2 == 0) {
				fpga_on[dot_location-1] += dot_check;
			}
			else if (dot_check2 == 1) {
				fpga_off[dot_location-1] -= dot_check;
			}
			pthread_mutex_lock(&mutex); // mutex lock
			mtime = 0;
			write(dev[3], &led_data, 1);
			pthread_mutex_unlock(&mutex); // mutex unlock
			dot_location--;
		}
		else if (push_sw_buff[3] && dot_flag < 7) {
			fpga_on[dot_location] = fpga_data[dot_location];
			fpga_off[dot_location] = fpga_data[dot_location]; 
			
			dot_flag++;
			dot_check2 = fpga_on[dot_location];
			for (i=0;i<dot_flag-1;i++) {
				dot_check *= 2;
				dot_check2 /= 2;
			}
			dot_check2 %= 2;
			
			if (dot_check2 == 0) {
				fpga_on[dot_location] += dot_check;
			}
			else if (dot_check2 == 1) {
				fpga_off[dot_location] -= dot_check;
			}
			pthread_mutex_lock(&mutex); // mutex lock
			mtime = 0;
			write(dev[3], &led_data, 1);
			pthread_mutex_unlock(&mutex); // mutex unlock
		}
		else if (push_sw_buff[5] && dot_flag > 1) {
			fpga_on[dot_location] = fpga_data[dot_location];
			fpga_off[dot_location] = fpga_data[dot_location]; 
			
			dot_flag--;
			dot_check2 = fpga_on[dot_location];
			for (i=0;i<dot_flag-1;i++) {
				dot_check *= 2;
				dot_check2 /= 2;
			}
			dot_check2 %= 2;
			
			if (dot_check2 == 0) {
				fpga_on[dot_location] += dot_check;
			}
			else if (dot_check2 == 1) {
				fpga_off[dot_location] -= dot_check;
			}
			pthread_mutex_lock(&mutex); // mutex lock
			mtime = 0;
			write(dev[3], &led_data, 1);
			pthread_mutex_unlock(&mutex); // mutex unlock
		}
		else if (push_sw_buff[7] && dot_location < 9) {
			fpga_on[dot_location] = fpga_data[dot_location];
			fpga_off[dot_location] = fpga_data[dot_location]; 
			
			dot_check2 = fpga_on[dot_location+1];
			for (i=0;i<dot_flag-1;i++) {
				dot_check *= 2;
				dot_check2 /= 2;
			}
			dot_check2 %= 2;
			
			if (dot_check2 == 0) {
				fpga_on[dot_location+1] += dot_check;
			}
			else if (dot_check2 == 1) {
				fpga_off[dot_location+1] -= dot_check;
			}
			pthread_mutex_lock(&mutex); // mutex lock
			mtime = 0;
			write(dev[3], &led_data, 1);
			pthread_mutex_unlock(&mutex); // mutex unlock
			dot_location++;
		}
		else if (push_sw_buff[4]) {
			dot_check2 = fpga_data[dot_location];
			for (i=0;i<dot_flag-1;i++) {
				dot_check2 /= 2;
			}
			dot_check2 %= 2;
			pthread_mutex_lock(&mutex); // mutex lock
			mtime = 0;
			write(dev[3], &led_data, 1);
			pthread_mutex_unlock(&mutex); // mutex unlock
			if (dot_check2 == 1) {
				bzr_data = 1;
				write(dev[6], &bzr_data, 1);
				sleep(1);
				bzr_data = 0;
				write(dev[6], &bzr_data, 1);
				continue;
			}
			else {
				write(dev[2], fpga_on, sizeof(fpga_on));
				exit_flag = 1;
				break;
			}
		}
		else {
			
		}
	}
	
	pthread_mutex_lock(&mutex); // mutex lock
	write(dev[3], &led_data, 1);
	pthread_mutex_unlock(&mutex); // mutex unlock
	
	motor_state[0] = (unsigned char)1;
	motor_state[1] = (unsigned char)1;
	motor_state[2] = (unsigned char)50;
	write(dev[4], motor_state, 3);
	
	fnd_cnt = 0;
	for (i=5;i<10;i++) {
		dot_check = fpga_on[i];
		for (j=0;j<7;j++) {
			if (dot_check % 2)
				fnd_cnt++;
			dot_check /= 2;
		}
	}
	
	fnd_data[0] = (unsigned char)fnd_cnt/10;
	fnd_data[1] = (unsigned char)fnd_cnt%10;
	fnd_data[2] = (unsigned char)((35-fnd_cnt)/10);
	fnd_data[3] = (unsigned char)((35-fnd_cnt)%10);
	write(dev[5], &fnd_data, 4);
	
	sleep(3);
	
	motor_state[0] = (unsigned char)0;
	motor_state[1] = (unsigned char)0;
	motor_state[2] = (unsigned char)0;
	write(dev[4], motor_state, 3);
	
	memset(lcd_string, 0, sizeof(lcd_string));
	lcd_str_size = strlen(movie_list[0]);
	strncat(lcd_string, movie_list[0], lcd_str_size);
	memset(lcd_string + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);
	
	strcat(cur_seat, " reserved!");
	lcd_str_size = strlen(cur_seat);
	strncat(lcd_string, cur_seat, lcd_str_size);
	memset(lcd_string + LINE_BUFF + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);
	write(dev[0], lcd_string, MAX_BUFF);
	
	// device close
	for (i=0;i<6;i++) {
		sleep(1);
		close(dev[i]);
	}
	return 0;
}

void * timeout(void * arg) 
{
	unsigned char led_read;
	int flag = 0;
	int lcd_str_size = 0;
	char lcd_string[32];
	
	while(1)
	{
		pthread_mutex_lock(&mutex); // mutex lock
		read(dev[3], &led_read, 1);
		pthread_mutex_unlock(&mutex); // mutex unlock
		
		if (mtime == 5 && flag%2 == 0) {
			led_read += 8;
		}
		else if (mtime == 10 && flag%2 == 0) {
			led_read += 4;
		}
		else if (mtime == 15 && flag%2 == 0) {
			led_read += 2;
		}
		else if (mtime >= 20 && flag%2 == 0) {
			led_read += 1;
			lcd_str_size = strlen("Time Out!!!");
			strncat(lcd_string, "Time Out!!!", lcd_str_size);
			memset(lcd_string + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);
	
			lcd_str_size = strlen("");
			strncat(lcd_string, "", lcd_str_size);
			memset(lcd_string + LINE_BUFF + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);
				
			write(dev[0], lcd_string, MAX_BUFF);
			exit_flag = 1;
			break;
		}
		pthread_mutex_lock(&mutex); // mutex lock
		write(dev[3], &led_read, 1);
		pthread_mutex_unlock(&mutex); // mutex unlock
		usleep(500000);
		
		flag++;
		if (flag%2 == 0)
			mtime++;
			
		if (exit_flag == 1)
			break;
	}
	return NULL;
}

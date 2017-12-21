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
#include <pthread.h>

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
// push_switch define
#define MAX_BUTTON 9

// step_motor define
#define MOTOR_ATTRIBUTE_ERROR_RANGE 4

// fnd define
#define MAX_DIGIT 4




//for push switch ..  
unsigned char quit = 0 ;
void user_signal1(int sig){
	quit = 1;
}

//push switch thread 
 void *pushSwitchThreadFunc(void *arg){
	int i;
	int dev;
	int buff_size;

	unsigned char push_sw_buff[MAX_BUTTON];

	dev = open(FPGA_PUSH_DEVICE, O_RDWR);

	if(dev<0){
		printf("Push Switch Open Error\n");
		close(dev);
		return -1;
	}

	(void)signal(SIGINT, user_signal1);
	buff_size = sizeof(push_sw_buff);
	printf("Press <ctrl+c> to quit. \n");
	while(!quit){
		usleep(400000);
		read(dev, &push_sw_buff, buff_size);

		for(i=0;i<MAX_BUTTON;i++){
			printf("[%d] " , push_sw_buff[i]);
		}
		printf("\n");
	}
	close(dev);
	return (void *)
}


int main(void)
{
	/* Device Values Define*/

	int menunum = 0; //메뉴 선택
	int myLotteryNum[6]; //나의 로또 번호
	int JackpotLotteryNum[6]; //당첨 로또 번호
	int i = 0, j = 0; //using for any loop
	char lcd_print_buf[2]; //LCD가 2줄로 출력됨
	int myrank = 6; //당첨번호랑 내 번호랑 비교해서 계산되는 등수임
	int flowflag = 0; //흐름 플래그

	//FPGA_STEP_MOTOR_DEVICE Set Value
	int motor_dev;
	int motor_action;
	int motor_direction;
	int motor_speed;
	unsigned char motor_state[3];


	//FPGA_TEXT_LCD_DEVICE Set Value
	int lcd_dev;
	int lcd_str_size;
	int lcd_chk_size;
	unsigned char lcd_string[32];


	//FPGA_PUSH_DEVICE Set Value 
	int push_sw_dev; //장치
	int push_buff_size; //사이즈
	unsigned char push_sw_buff[MAX_BUTTON]; //각 배열의 index마다 버튼값이 지정. 누르면 1 안누르면 0으로 설정 될 배열 선언
	//int retval;


/* Device Init Setting */

	//FPGA_STEP_MOTOR_DEVICE Init 
	memset(motor_state, 0, sizeof(motor_state));
	motor_dev = open(FPGA_STEP_MOTOR_DEVICE, O_WRONLY);
	if (motor_dev < 0) {
		printf("Device open error : %s\n", FPGA_STEP_MOTOR_DEVICE);
		exit(1);
	}
	motor_state[0] = (unsigned char)0;
	motor_state[1] = (unsigned char)0;
	motor_state[2] = (unsigned char)0;

	//FPGA_TEXT_LCD_DEVICE Init 
	memset(lcd_string, 0, sizeof(lcd_string));
	lcd_dev = open(FPGA_TEXT_LCD_DEVICE, O_WRONLY);
	if (lcd_dev < 0) {
		printf("Device open error : %s\n", FPGA_TEXT_LCD_DEVICE);
		return -1;
	}

	//FPGA_PUSH_SWITCH  init 
	push_sw_dev = open(FPGA_PUSH_DEVICE, O_RDWR);
	if (push_sw_dev < 0) {
		printf("push switch device open error\n");
		close(push_sw_dev);
		return -1;
	}
	push_buff_size = sizeof(push_sw_buff); // push_sw_buff size



/* Application */

	//intro 3sec and play app

	//start lottery
	memset(lcd_string, 0, sizeof(lcd_string));
	lcd_str_size = strlen("One Shot Life!");
	strncat(lcd_string, "One Shot Life!", lcd_str_size);
	memset(lcd_string + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);

	lcd_str_size = strlen("Hi! Lucky5250");
	strncat(lcd_string, "Hi! Lucky5250", lcd_str_size);
	memset(lcd_string + LINE_BUFF + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);
	write(lcd_dev, lcd_string, MAX_BUFF);
	sleep(5);

	//Using Thie Program
	while (1) {
		//Menu
		// Device - LCD, Push_Switch
		/*
		print LCD
		"1.Self
		 2.Auto
		 3.Check Jackpot
		*/
		if(menunum == 0){
			memset(lcd_string, 0, sizeof(lcd_string));
			lcd_str_size = strlen("1.Self 2.Auto");
			strncat(lcd_string, "1.Self 2.Auto", lcd_str_size);
			memset(lcd_string + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);
	
			lcd_str_size = strlen("3.CheckJackpot");
			strncat(lcd_string, "3.CheckJackpot", lcd_str_size);
			memset(lcd_string + LINE_BUFF + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);
			write(lcd_dev, lcd_string, MAX_BUFF);
	
			//input menu
			//TODO : switch testing................ have to fix it
			read(push_sw_dev, &push_sw_buff, push_buff_size);
			if (push_sw_buff[0]) {// 1x1
				menunum = 0;
				push_sw_buff[0] == 0;
			}
			else if (push_sw_buff[1]) {//1x2
				menunum = 1;
				push_sw_buff[1] == 0;
			}
			else if (push_sw_buff[2]) {//1x3
				menunum = 2;
				push_sw_buff[2] == 0;
			}
			else {// other push_sw handling
				menunum = 0;
				continue;
			}
		}


		//excute menu
		switch (menunum) {
		case 1: //self select lottery number
			//TODO : input push_Sw & show lcd  현명 파트
			//show lcd sample



			i = 0;
			j = 0;
			menunum = 0;
			break;

		case 2: //Auto select lotterr number
				//Motor Use
				// Motor Spin 3sec - Do Wirte FPGA_STEP_MOTOR_DEVICE
				if(flowflag == 0){// flowflag=0 : 자동으로 번호를 선택하는 상태
					motor_state[0] = (unsigned char)1;
					motor_state[1] = (unsigned char)1;
					motor_state[2] = (unsigned char)50;
					write(motor_dev, motor_state, 3);
					sleep(5);
					/*
					*	Random Select 6 numbers
					*/
					//pick 6 numbers
					for (i = 0; i < 6; i++) {
						myLotteryNum[i] = (rand() % 45) + 1;
					}
					//check for Redundancy number
					for (i = 0; i < 6; i++) {
						for (j = 0; j < 6; j++) {
							if (myLotteryNum[j] == myLotteryNum[i] && i != j) {
								myLotteryNum[i] = (rand() % 45) + 1;
								j = 0;
							}
						}
					}
					// Motor Stop - Do Wirte FPGA_STEP_MOTOR_DEVICE
					motor_state[0] = (unsigned char)0;
					motor_state[1] = (unsigned char)0;
					motor_state[2] = (unsigned char)0;
					write(motor_dev, motor_state, 3);
				
					//LCD Use
					// init memset
					memset(lcd_string, 0, sizeof(lcd_string));
					
					// show Line 1
					//convert int to string for lcd print
					buf[0] = '\0';
					lcd_string[0] = '\0';
					for (i = 0; i < 5; i++) {
						sprintf(buf, "%d", JackpotLotteryNum[i]);
						strcat(lcd_string, buf);
	
						j = strlen(lcd_string);
						if (i != 4) {
							
							lcd_string[j] = ',';
							lcd_string[j + 1] = '\0';
							j = 0;
						}
						buf[0] = '\0';
						lcd_string[j] = '\0';
					}
					lcd_str_size = strlen(lcd_string);
					memset(lcd_string + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);
	
					//show Line 2
					lcd_str_size = strlen("Lucky No.6 : ");
					strncat(lcd_string, "Lucky No.6 : ", lcd_str_size);
					sprintf(buf, "%d", JackpotLotteryNum[5]);
					strcat(lcd_string, buf);
					lcd_str_size = strlen(lcd_string);
					memset(lcd_string + LINE_BUFF + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);
					flowflag = 1;
				}
				if(flowflag == 1){// flowflag=1 : 번호 선택이 끝나고 번호를 출력하면서 0번버튼의 입력을 기다린다.
					//LCD write string
					write(lcd_dev, lcd_string, MAX_BUFF);
					
					read(push_sw_dev, &push_sw_buff, push_buff_size);
					if (push_sw_buff[0]) {// 1x1
						i = 0;
						j = 0;
						menunum = 0;
						push_sw_buff[0] == 0;
						flowflag = 0;
					}
				}

			break;

		case 3: //check Jackpot number & compare with my number
			/*
			*	Random Select 6 numbers of Jackpot
			*/
			//pick 6 numbers
			for (i = 0; i < 6; i++) {
				JackpotLotteryNum[i] = (rand() % 45) + 1;
			}
			//check for Redundancy number
			myrank = 6;
			for (i = 0; i < 6; i++) {//compare jackpot with mynumbers
				for (j = 0; j < 6; j++) {	
					if (myLotteryNum[i] == JackpotLotteryNum[j]) {	
						myrank -= 1;
					}
				}
			}
			


			i = 0;
			j = 0;
			menunum = 0;
			break;

		}
	}

	printf("나의 번호 확인\n");
	for (i = 0; i < 6; i++) {
		printf("%d = %d\n", i + 1, JackpotLotteryNum[i]);
	}



	/*How to use Device IO*/
	/*
	//Motor Use
	// Motor Spin 3sec - Do Wirte FPGA_STEP_MOTOR_DEVICE
	motor_state[0]=(unsigned char)1;
    motor_state[1]=(unsigned char)1;
    motor_state[2]=(unsigned char)50;
	write(motor_dev,motor_state,3);
	sleep(3);
	// Motor Stop - Do Wirte FPGA_STEP_MOTOR_DEVICE
    motor_state[0]=(unsigned char)0;
    motor_state[1]=(unsigned char)0;
    motor_state[2]=(unsigned char)0;
    write(motor_dev,motor_state,3);


	//LCD Use
	// init memset
	memset(lcd_string, 0, sizeof(lcd_string));
	// show Line 1
	lcd_str_size = strlen("One Shot Life!");
	strncat(lcd_string, "One Shot Life!", lcd_str_size);
	memset(lcd_string + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);
	//show Line 2
	lcd_str_size = strlen("Hi! Lucky5250");
	strncat(lcd_string, "Hi! Lucky5250" , lcd_str_size);
	memset(lcd_string + LINE_BUFF + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);
        //write string
	write(lcd_dev, lcd_string, MAX_BUFF);

	//RANDOM choose number 1~45
	(rand() % 45) + 1;
	sleep(1);

	//Push Switch Use
	read(push_sw_dev, &push_sw_buff, push_buff_size);
	*/

/* Device Close */
	close(motor_dev);
	close(lcd_dev);	
	close(push_sw_dev);	


	return 0;
}

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
// push_switch define
#define MAX_BUTTON 9

// step_motor define
#define MOTOR_ATTRIBUTE_ERROR_RANGE 4

// fnd define
#define MAX_DIGIT 4


int main(void)
{
	/* Device Values Define*/

	int menunum = 0;
	int myLotteryNum[6];
	int JackpotLotteryNum[6];
	int i = 0, j = 0; //using for any loop
	char lcd_print_buf[2];
	int myrank = 6;
	int flowflag = 0;

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
	int push_sw_dev;
	int push_buff_size;
	unsigned char push_sw_buff[MAX_BUTTON];
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


		if(LottEmptyCheck == 7)
			break;

		//excute menu
		switch (menunum) {
		case 1: //self select lottery number
			//TODO : input push_Sw & show lcd
			//show lcd sample


			for(i=0 ; i< 6; i++){
				if(push_sw_buff[0]= 1){ //10 up 
					if(lottoNum <= 45 && lottoNum >0){ 
						myLotteryNum[i] += 10;
					}
					push_sw_buff[0] = 0;
	
				} else if(push_sw_buff[1]== 1){ //1 up
					
					if(lottoNum <= 45 && lottoNum >0){ 
						myLotteryNum[i] += 1;
					}
					push_sw_buff[1] = 0;
	
				} else if(push_sw_buff[2] == 1){ // comfirm
					if(myLotteryNum[i] != 0){
						LottEmptyCheck++;
					}
					push_sw_buff[2] = 0;
	
				} else if(push_sw_buff[3]  == 1){ // 10 down
					if(myLotteryNum[i] - 10 > 0 && myLotteryNum[i] - 10 <=45)
						myLotteryNum[i] -= 10;
					push_sw_buff[3] = 0;
	
				} else if(push_sw_buff[4]  == 1){ // 1 down
					if(myLotteryNum[i] - 1 > 0 && myLotteryNum[i] - 1 <=45)
						myLotteryNum[i] -= 1;
					push_sw_buff[4] = 0;
				}
			}
				
			i = 0;
			j = 0;
			menunum = 0;
			break;

		case 2: //Auto select lotterr number
				if(flowflag == 0){// flowflag=0 : 자동으로 번호를 선택하는 상태
					//Motor Use
					// Motor Spin 3sec - Do Wirte FPGA_STEP_MOTOR_DEVICE
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
						sprintf(buf, "%d", myLotteryNum[i]);
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

			break;//end of case 2 : menu2

		case 3: //check Jackpot number & compare with my number
				if(flowflag == 0){// flowflag=0 : 자동으로 번호를 선택하는 상태
					//Motor Use
					// Motor Spin 3sec - Do Wirte FPGA_STEP_MOTOR_DEVICE
					motor_state[0] = (unsigned char)1;
					motor_state[1] = (unsigned char)1;
					motor_state[2] = (unsigned char)50;
					write(motor_dev, motor_state, 3);
					sleep(5);
					//check Jackpot number 
					/*
					*	Random Select 6 numbers
					*/
					//pick 6 numbers
					for (i = 0; i < 6; i++) {
						JackpotLotteryNum[i] = (rand() % 45) + 1;
					}
					//check for Redundancy number
					for (i = 0; i < 6; i++) {
						for (j = 0; j < 6; j++) {
							if (JackpotLotteryNum[j] == JackpotLotteryNum[i] && i != j) {
								JackpotLotteryNum[i] = (rand() % 45) + 1;
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
					sleep(5);
					

					//compare jackpot with mynumber
					myrank = 6;
					for (i = 0; i < 6; i++) {
						for (j = 0; j < 6; j++) {
							if (myLotteryNum[i] == JackpotLotteryNum[j]) {
								myrank -= 1;
							}
						}
					}
					//print lcd my rank
					//show Line1
					memset(lcd_string, 0, sizeof(lcd_string));
					lcd_str_size = strlen("My Number Rank");
					strncat(lcd_string, "My Number Rank", lcd_str_size);
					memset(lcd_string + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);
	
					//show Line 2
					lcd_str_size = strlen("Lucky Rank : ");
					strncat(lcd_string, "Lucky Rank : ", lcd_str_size);
					sprintf(buf, "%d", myrank);
					strcat(lcd_string, buf);
					lcd_str_size = strlen(lcd_string);
					memset(lcd_string + LINE_BUFF + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);
					sleep(5);
					
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
						myrank = 6;
						push_sw_buff[0] == 0;
						flowflag = 0;
					}
				}		

			break;//end of case 3 : menu3

		}//end of menu's switch case
	}//end of program main logic while()

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

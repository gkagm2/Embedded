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

// Driver define
#define FPGA_TEXT_LCD_DEVICE "/dev/fpga_text_lcd"
#define FPGA_PUSH_DEVICE "/dev/fpga_push_switch"
#define FPGA_DOT_DEVICE "/dev/fpga_dot"
#define FPGA_FND_DEVICE "/dev/fpga_fnd"
#define FPGA_STEP_MOTOR_DEVICE "/dev/fpga_step_motor"

// driver amount
#define MAX_DEVICE 5

// text lcd define
#define MAX_BUFF 32
#define LINE_BUFF 16

// push_switch define
#define MAX_BUTTON 9

// step_motor define
#define MOTOR_ATTRIBUTE_ERROR_RANGE 4

// fnd define
#define MAX_DIGIT 4




int main(int argc, char **agrgv)
{
/*******************************************************************************
 *
 *
 * Device Values Define
 * 
 *
*******************************************************************************/
        int menunum = 0;
        int myLotteryNum[6];
        int JackpotLotteryNum[6];

        /*
         * FPGA_STEP_MOTOR_DEVICE Set Value
         */
        int motor_dev;
        int motor_action;
        int motor_direction;
        int motor_speed;
        unsigned char motor_state;
                                                        
        /*
         * FPGA_TEXT_LCD_DEVICE Set Value
         */
        int lcd_dev;
        int lcd_str_size;
        int lcd_chk_size;
        unsigned char lcd_string[32];

        /*
         * FPGA_PUSH_DEVICE Set Value
         */
        int push_sw_dev;
        int push_buff_size;
        unsigned char push_sw_buff[MAX_BUTTON];



/*******************************************************************************
 *
 *
 * Device Init Setting
 * 
 *
*******************************************************************************/

        /*
         * FPGA_STEP_MOTOR_DEVICE Init 
         */
        memset(motor_state,0,sizeof(motor_state));
        motor_state[0]=(unsigned char)0;
        motor_state[1]=(unsigned char)0;
        motor_state[2]=(unsigned char)0;
        motor_dev = open(FPGA_STEP_MOTOR_DEVICE, O_WRONLY);
        if (motor_dev<0) {
                printf("Device open error : %s\n",FPGA_STEP_MOTOR_DEVICE);
                exit(1);
        }


        /*
         * FPGA_TEXT_LCD_DEVICE Init 
         */
        memset(lcd_string, 0, sizeof(lcd_string));
        lcd_dev = open(FPGA_TEXT_LCD_DEVICE, O_WRONLY);
        if (lcd_dev<0) {
                printf("Device open error : %s\n",FPGA_TEXT_LCD_DEVICE);
                return -1;
        }


        /*
         * FPGA_TEXT_LCD_DEVICE Init 
         */
        push_buff_size=sizeof(push_sw_buff); // push_sw_buff size

/*******************************************************************************
 *
 *
 * Application
 * 
 *
*******************************************************************************/

        //start lottery
        memset(lcd_string, 0, sizeof(lcd_string));
        lcd_str_size = strlen("One Shot Life!");
        strncat(lcd_string, "One Shot Life!", lcd_str_size);
        memset(lcd_string + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);

        lcd_str_size = strlen("Hi! Lucky5250");
        strncat(lcd_string, "Hi! Lucky5250" , lcd_str_size);
        memset(lcd_string + LINE_BUFF + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);
        write(lcd_dev, lcd_string, MAX_BUFF);
        //intro 3sec and play app       
        usleep(3000000);
        memset(lcd_string, 0, sizeof(lcd_string));

        while(1){

                //Menu
                // Device - LCD, Push_Switch
                /*
                print LCD
                "1.Self
                 2.Auto
                 3.Check Jackpot        
                */
                memset(lcd_string, 0, sizeof(lcd_string));
                lcd_str_size = strlen("1.Self 2.Auto");
                strncat(lcd_string, "1.Self 2.Auto", lcd_str_size);
                memset(lcd_string + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);

                lcd_str_size = strlen("3.CheckJackpot");
                strncat(lcd_string, "3.CheckJackpot" , lcd_str_size);
                memset(lcd_string + LINE_BUFF + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);
                write(lcd_dev, lcd_string, MAX_BUFF);

                //input menu
                read(push_sw_dev, &push_sw_buff, push_buff_size);
                if (push_sw_buff[1]) {
                        menunum = 1;
                }
                else if (push_sw_buff[3]) {
                        menunum = 2;

                }
                else if (push_sw_buff[7]) {
                        menunum = 3;
                }
                else{// other push_sw handling
                        menunum = 0;
                        continue;
                }

                (rand() % 45) + 1;


//excute menu
                switch(menunum){
                        case 1: //self select lottery number
                                //TODO : input push_Sw & show lcd
                                //show lcd sample
                                memset(lcd_string, 0, sizeof(lcd_string));
                                lcd_str_size = strlen("One Shot Life!");
                                strncat(lcd_string, "One Shot Life!", lcd_str_size);
                                memset(lcd_string + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);

                                lcd_str_size = strlen("Hi! Lucky5250");
                                strncat(lcd_string, "Hi! Lucky5250" , lcd_str_size);
                                memset(lcd_string + LINE_BUFF + lcd_str_size, ' ', LINE_BUFF - lcd_str_size);
                                write(lcd_dev, lcd_string, MAX_BUFF);
                                break;

                        case 2: //Auto select lotterr number

                                break;

                        case 3: //check Jackpot number & compare with my number

                                break;

                }
        }






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


        //Push Switch Use
        read(push_sw_dev, &push_sw_buff, push_buff_size);


/*******************************************************************************
 *
 *
 * Device Close
 * 
 *
*******************************************************************************/
        close(motor_dev);
        close(lcd_dev);
        close(push_sw_dev);


        return 0;
}



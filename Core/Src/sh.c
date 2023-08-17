#include "main.h"
#include "sh.h"
#include "dma.h"
#include "i2c.h"
#include  "spi.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"
#include "Adafruit_ST7735.h"
#include "gfxfont.h"
#include "graphics.h"
//#include <stdio.h>



int shell() 
{
    int i;
    char *echo = "echo";
    char input_chars[100]; //more like input_chars[100]
    while (1) 
    {
	printf("\n\r");
        printf("$: "); //print  $: which lets the user know the shell is ready
        get_input_line(input_chars); //get the number of inputs chars
        // Check if the input line starts with the keyword "echo"
        for (i = 0; i < 4; i++)
	{ //checking to see if the first fe characters are echo
            if (input_chars[i] != *(echo + 1))
	    {
                break;
            }

        }
        int st = 5; //starting point at 5 because echo is 4
        while (input_chars[st] == ' ') 
	{
                st++; //increment start to get any other spaces after echo out
        }
            printf("%s\n", &input_chars[st]);
    }
    return 0;
}

// Function to get a line of input from the user
int get_input_line(char *input_chars) 
{
    int i = 0;
    char h;
   // setvbuf(stdout, NULL, _IONBF, 0);//disable output buffering
    while (i < 100) 
    {//make sure i doesn't go overboard
	 h = getchar(); //getting characters from user regardless of what it is, and returns it to the program
	 if (h == '\b')
	 {
		 if (i > 0)
		 { 
			i--;
			printf("\b \b"); //outputs two backslashes
		 }		
	 } 
           // End the input line if a newline or carriage return is detected
	 else if (h == '\n' || h == '\r') 
	 { //if a new line is hit
            input_chars[i] = '\0';
            printf("\n\r"); //prints a new line
            break; //then ends
         }
	else 
	{
            input_chars[i] = h; //then place place all the char c in array
            i++; //incrementing i
            printf("%c", h); 
          //  fflush(stdout); 

	}

     //  fflush(stdout); 

    }
    return i;
}


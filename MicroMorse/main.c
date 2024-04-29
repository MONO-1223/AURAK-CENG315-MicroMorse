#include "stm32l476xx.h" // provides access to register definitions and other hardware-specific information
#include "lcd.h" // a custom header file containing function prototypes, macros, and other definitions related to an LCD module
#include <stdio.h>
#include <string.h>
#include <ctype.h>

extern void my_asm_main(void); // Declare the prototype of the assembly function

 char morse_input[500] = "";
 char english_input[26] = "";
 char button_functions_input[3] = "";
 static const char* morse_code[] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.."};
 static const char* characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void System_Clock_Init(void)
{
	// Enable the Internal High-Speed oscillator (HSI) by setting the HSION bit in the RCC_CR register.
	
	RCC->CR |= RCC_CR_HSION; 
	while((RCC->CR & RCC_CR_HSIRDY) == 0);
}

void ms_delay(volatile int ms) { 
	// This function provides a delay in milliseconds. 
    while (ms-- > 0) {
        volatile int x = 700;
        while (x-- > 0)
            __NOP(); // no operation; do nothing
    }
}

void scroll_text(const uint8_t* text) {
	// This function achieves the scroll effect by iteratively displaying each character of the input text string 
	// on the LCD screen and introducing a delay between each character to control the scrolling speed.
    uint8_t length = strlen((char*)text);
    
    for (uint8_t i = 0; i <= length; ++i) {
        LCD_DisplayString(&text[i], 0);
        ms_delay(100); // control scrolling speed
    }
}

char morse_to_char(const char* morse) {
	// This function searches through a table of Morse code representations for letters of the alphabet and returns 
	// the index of the corresponding letter when given its Morse code. If the Morse code is not found in the table, 
	// it returns -1 to indicate that the Morse code is invalid or not recognized.
    int i;
    for (i = 0; i < 26; ++i) {
        if (strcmp(morse, morse_code[i]) == 0) {
            return i;
        }
    }
    return -1; // Return null character if Morse code is not found
}

const char* char_to_morse(char c) {
	// This function converts an English character to its corresponding Morse code representation if the 
	// character is in the range 'A' to 'Z'. If the character is not within this range, it returns NULL.
    c = toupper(c); // Convert character to uppercase
    
    // Check if the character is in the range 'A' to 'Z'
    if (c >= 'A' && c <= 'Z') {
        return morse_code[c - 'A'];
    }
		else {
        return NULL; // Return NULL for characters not in the Morse code lookup table
    }
}

void print_english(const char* morse_code) {
	  LCD_Clear();
    // Define delimiter for tokenizing
    const char* delimiter = "+";
    
    // Initialize result string with space for 6 characters + null terminator
    char result[100] = "";

    // Tokenize the Morse code string using space as the delimiter
    char* token = strtok((char*)morse_code, delimiter); // strtok is used to tokenize (split) a string into smaller strings (tokens) based on a delimiter.
    while (token != NULL) {
            // Convert the Morse code to character index and append the corresponding character to the result string
			// Check if the token is a forward slash
        if (strcmp(token, "/") == 0) {
            // If it's a forward slash, interpret it as a space character
            strncat(result, " ", 1);
        } else {
            int index = morse_to_char(token);
            if (index >= 0 && index < 26) {
                char character = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[index];
                strncat(result, &character, 1);
						}else {
                // Invalid Morse code encountered, display message and return
                LCD_Clear();
                scroll_text((uint8_t*)"    INVALID INPUT  ");
                Joystick_Listen1_after_invalid();
                return; // Stop processing further tokens
            }
			}
        
        // Get the next token
        token = strtok(NULL, delimiter);
    }
		
		// Add a space at the end of the result string
    strncat(result, " ", 1);
    
    // Fill the result string with spaces if it has less than 6 characters
    size_t result_length = strlen(result);
    if (result_length < 6) {
        strncat(result, "      ", 6 - result_length); // Add enough spaces to fill up to 6 characters
    }
		
		// Add a terminator at the end of the result string
    strncat(result, "\0", 1);
		    
// Check the length of the result string
    if (result_length <= 6) {
        // Display the result string on the LCD if it has 6 or fewer characters
        LCD_DisplayString((uint8_t*)result, 0);
    } else {
        // Scroll the result string if it has more than 6 characters
			  for (int i = 0; i < 3; ++i) {
        scroll_text((uint8_t*)result);	
    }
}
}

void print_morse(char english_char) {
	// This function converts an English character to Morse code using the char_to_morse function, appends the Morse code 
	// representation to a character array, adds a space at the end of the Morse code representations of different characters,
  // to avoid printing unpredictable characters on the LCD, displays the Morse code on an LCD, introduces a delay, and then listens for joystick input.
    char morse_code[5] = "";

    const char* current_morse = char_to_morse(english_char);
        if (current_morse != NULL) {
            strcat(morse_code, current_morse); // Append the Morse code to the morse_code array
            strcat(morse_code, " "); // Add a space between Morse code representations of different characters
        }

    // Display the Morse code on the LCD
    LCD_DisplayString((uint8_t*)morse_code, 0);
		ms_delay(800);
    Joystick_Listen2();
}

void ok_button(const char* button_functions_input) {
	// This function handles right button inputs and displays corresponding instructions on an LCD screen. 
	// Depending on the length of the input string, it displays different sets of instructions, clears the LCD after a 
	// delay, and redirects the program control back to listen to further input from the joysick.
    size_t length = strlen(button_functions_input);
    if (length == 1) {
			  LCD_DisplayString((uint8_t*)"HELP", 0);
			  ms_delay(200);
				LCD_Clear();
			  scroll_text((uint8_t*)"    PRESS RIGHT 1 TIME FOR HELP ");
				LCD_Clear();
			  scroll_text((uint8_t*)"    PRESS RIGHT 2 TIMES FOR MODE 1 - ADDING DELIMITER ");
				LCD_Clear();
				scroll_text((uint8_t*)"    PRESS RIGHT 3 TIMES FOR MODE 2 - ADDING SPACE ");
				LCD_Clear();
			  scroll_text((uint8_t*)"    PRESS RIGHT 4 TIMES FOR MODE 3 - SWITCH TRANSLATION MODE TO ENG-MORSE ");
				LCD_Clear();
			  scroll_text((uint8_t*)"    AFTER CHOOSING RIGHT MODE PRESS OK ");
				LCD_Clear();
			  scroll_text((uint8_t*)"    LEFT IS AN OK BUTTON ");
				LCD_Clear();
			  scroll_text((uint8_t*)"    UP FOR MORSE DOT ");
				LCD_Clear();
			  scroll_text((uint8_t*)"    DOWN FOR MORSE DASH ");
				LCD_Clear();
			  scroll_text((uint8_t*)"    PRESS MIDDLE 1 TIME TO TRANSLATE ");
				LCD_Clear();
			  scroll_text((uint8_t*)"    PRESS MIDDLE 2 TIMES TO CLEAR LCD SCREEN ");
				LCD_Clear();
			  scroll_text((uint8_t*)"    IN MODE 3: PRESS UP TO INCREMENET ENGLISH ALPHABET ");
				LCD_Clear();
				scroll_text((uint8_t*)"    IN MODE 3: MIDDLE TO TRANSLATE ");
				LCD_Clear();
				scroll_text((uint8_t*)"    IN MODE 3: DOWN TO SWITCH TRANSLATION MODE TO MORSE-ENGLISH ");
				LCD_Clear();
				scroll_text((uint8_t*)"    PROJECT BY MONO ");
				LCD_Clear();
    } else if (length == 2) {
				LCD_DisplayString((uint8_t*)"MODE 1", 0);
        strcat(morse_input, "+");
    } else if (length == 3) {
				LCD_DisplayString((uint8_t*)"MODE 2", 0);
				strcat(morse_input, "/"); // space between words on the lcd
    } else if (length == 4) {
				LCD_DisplayString((uint8_t*)"MODE 3", 0);
			  ms_delay(200);
        Joystick_Listen2();
    }
		
		ms_delay(200);
		LCD_Clear();
		memset(button_functions_input, 0, sizeof(button_functions_input)); //  function in C that is used to fill a block of memory with a particular value
		Joystick_Listen1();
}

Joystick_Listen1(){
	// This function listens for joystick input and performs corresponding actions based on the buttons pressed. 
	// It allows users to input Morse code characters using the joystick buttons, display the Morse code on an LCD, 
	// translate the Morse code to English, and handle other functionalities such as adding spaces between words and characters.
	  LCD_Clear();
	  uint8_t current_position = 0; // Initialize current position on the LCD
	  while(1){
    
		// If middle button is pushed, translate morse to english and display on lcd
		if((GPIOA->IDR & 0x1) != 0x00){
			current_position = 0;
      print_english(morse_input); 
      memset(morse_input, 0, sizeof(morse_input)); // Clear morse_input array
			while((GPIOA->IDR & 0x1) != 0x00);
		}
		
		// If left button is pushed, implement space between words
		if((GPIOA->IDR & 0x2) != 0x00){
			ok_button(button_functions_input);
			while((GPIOA->IDR & 0x2) != 0x00);
		}
		
		// If right button is pushed, implement space between characters 
		if((GPIOA->IDR & 0x4) != 0x00){
		strcat(button_functions_input, ".");
			while((GPIOA->IDR & 0x4) != 0x00);
		}
		
		// If up button is pushed, send a dot (.) to the morse_input array
		if((GPIOA->IDR & 0x8) != 0x00){
			LCD_DisplayString((uint8_t*)".", current_position);
			current_position++; // Move to the next position on the LCD
		  strcat(morse_input, ".");
			while((GPIOA->IDR & 0x8) != 0x00);
		}
		
		// If down button is pushed, send a dash (-) to the morse_input array
		if((GPIOA->IDR & 0x20) != 0x00){
		 LCD_DisplayString((uint8_t*)"-", current_position);
		 current_position++; // Move to the next position on the LCD
		 strcat(morse_input, "-");
			while((GPIOA->IDR & 0x20) != 0x00);
		}
	
	}
}

Joystick_Listen1_after_invalid(){
	  LCD_Clear();
	  memset(morse_input, 0, sizeof(morse_input)); // Clear morse_input array
	  uint8_t current_position = 0; // Initialize current position on the LCD
	  while(1){
    
		// If middle button is pushed, translate morse to english and display on lcd
		if((GPIOA->IDR & 0x1) != 0x00){
			current_position = 0;
      print_english(morse_input); 
      memset(morse_input, 0, sizeof(morse_input)); // Clear morse_input array
			while((GPIOA->IDR & 0x1) != 0x00);
		}
		
		// If left button is pushed, implement space between words
		if((GPIOA->IDR & 0x2) != 0x00){
			ok_button(button_functions_input);
			while((GPIOA->IDR & 0x2) != 0x00);
		}
		
		// If right button is pushed, implement space between characters 
		if((GPIOA->IDR & 0x4) != 0x00){
		strcat(button_functions_input, ".");
			while((GPIOA->IDR & 0x4) != 0x00);
		}
		
		// If up button is pushed, send a dot (.) to the morse_input array
		if((GPIOA->IDR & 0x8) != 0x00){
			LCD_DisplayString((uint8_t*)".", current_position);
			current_position++; // Move to the next position on the LCD
		  strcat(morse_input, ".");
			while((GPIOA->IDR & 0x8) != 0x00);
		}
		
		// If down button is pushed, send a dash (-) to the morse_input array
		if((GPIOA->IDR & 0x20) != 0x00){
		 LCD_DisplayString((uint8_t*)"-", current_position);
		 current_position++; // Move to the next position on the LCD
		 strcat(morse_input, "-");
			while((GPIOA->IDR & 0x20) != 0x00);
		}
	
	}
}


Joystick_Listen2(){
	// This function listens for joystick input and performs different actions based on the buttons pressed. 
	// It allows users to increment through the English alphabet using the up button, translate Morse code to English 
	// and display it on an LCD using the middle button, decrement through the English alphabet using the down button, 
	// and switch back to morse to english mode using the left button.
	uint8_t up_press_count = 0;
    
    while(1) {
			  // If up is pushed
        if ((GPIOA->IDR & 0x8) != 0x00) {
					LCD_Clear();
            up_press_count++;
            if (up_press_count > 26) {
                up_press_count = 1;
            }
            char english_char[6] = {'A' + (up_press_count - 1), ' ', ' ', ' ', ' ', '\0'};
						
            size_t english_length = strlen(english_char);
						if (english_length < 6) {
								strncat(english_char, "      ", 6 - english_length); // Add enough spaces to fill up to 6 characters
						}
						
            LCD_DisplayString((uint8_t*)&english_char, 0);
            while ((GPIOA->IDR & 0x8) != 0x00);
        }
        
				// If middle is pushed
        if ((GPIOA->IDR & 0x1) != 0x00) {
					LCD_Clear();
          print_morse('A' + (up_press_count - 1)); 
          up_press_count = 0; // Reset the count to zero after translation
            while ((GPIOA->IDR & 0x1) != 0x00);
        }
        
				// If down button is pushed
				if((GPIOA->IDR & 0x20) != 0x00){
					LCD_Clear();
				up_press_count--;
            if (up_press_count < 1) {
                up_press_count = 26;
            }
            char english_char[6] = {'A' + (up_press_count - 1), ' ', ' ', ' ', ' ', '\0'};
						
            size_t english_length = strlen(english_char);
						if (english_length < 6) {
								strncat(english_char, "      ", 6 - english_length); // Add enough spaces to fill up to 6 characters
						}
						
            LCD_DisplayString((uint8_t*)&english_char, 0);
					while((GPIOA->IDR & 0x20) != 0x00);
				}
				
				// If left is pushed
				if((GPIOA->IDR & 0x2) != 0x00){
					LCD_DisplayString((uint8_t*)"MODE 4", 0);
					ms_delay(200);
					Joystick_Listen1();
			while((GPIOA->IDR & 0x2) != 0x00);
		}
    }
}

int main(void){	// this is the entry point of the program
	LCD_Initialization();
	LCD_Clear();
	scroll_text((uint8_t*)"    WELCOME TO MICRO-MORSE "); // aesthetic enhancement
	// there is space at the beginning because if the first word starts at the first digit of the lcd the scrol will happen too fast for the user's eye to have time to catch it and read
	LCD_Clear();
	
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	GPIOA->MODER &= ~0xCFF;
	GPIOA->PUPDR &= ~0xCFF;
	GPIOA->PUPDR |= 0x8AA;


	for (int i = 0; i < 6; i++) {
        my_asm_main(); // Call the assembly function - turn on green led
        ms_delay(100); // Delay for 100 milliseconds
        GPIOE->ODR &= ~GPIO_ODR_ODR_8; // turn off green led
    }	
	
	Joystick_Listen1();
}
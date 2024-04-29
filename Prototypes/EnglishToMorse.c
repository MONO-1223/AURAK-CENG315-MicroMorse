#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Function declarations
const char* char_to_morse(char c);
void print_morse(const char* text);

int main() {
    char english_input[500];
    
    printf("Enter English text (uppercase letters only):\n");
    fgets(english_input, sizeof(english_input), stdin);
    english_input[strcspn(english_input, "\n")] = '\0'; // Remove newline character
    
    printf("English Input: %s\n", english_input);
    printf("Morse Code Output: ");
    print_morse(english_input);
    
    return 0;
}

const char* char_to_morse(char c) {
    static const char* morse_code[] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..", "/"};
    static const char* characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
    
    c = toupper(c); // Convert character to uppercase
    
    // Check if the character is in the range 'A' to 'Z'
    if (c >= 'A' && c <= 'Z') {
        return morse_code[c - 'A'];
    } else if (c == ' ') {
        return morse_code[26]; // Use index 26 for space
    } else {
        return NULL; // Return NULL for characters not in the Morse code lookup table
    }
}

void print_morse(const char* text) {
    int i = 0;
    while (text[i] != '\0') {
        const char* morse = char_to_morse(text[i]);
        if (morse != NULL) {
            printf("%s ", morse);
        } else {
            printf(" ");
        }
        i++;
    }
    printf("\n");
}

#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Function declarations
int morse_to_index(const char* morse);
void print_english(const char* morse_code);

int main() {
    char morse_input[500];
    
    printf("Enter Morse code (use dots for '.' and dashes for '-'), with spaces between characters and forward slashes between words:\n");
    fgets(morse_input, sizeof(morse_input), stdin);
    morse_input[strcspn(morse_input, "\n")] = '\0'; // Remove newline character
    
    printf("Morse Code Input: %s\n", morse_input);
    printf("English Output: ");
    print_english(morse_input);
    
    return 0;
}

int morse_to_index(const char* morse) {
    static const char* morse_code[] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..", "/"};
    for (int i = 0; i < 27; ++i) {
        if (strcmp(morse, morse_code[i]) == 0) {
            return i;
        }
    }
    return -1; // Return -1 if not found
}

void print_english(const char* morse_code) {
    const char* morse_code_table[] = {
        ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", 
        "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-", 
        "..-", "...-", ".--", "-..-", "-.--", "--..", "/"
    };

    const char* word_delim = "/";
    const char* char_delim = " ";

    // Find the length of the Morse code input
    size_t len = strlen(morse_code);

    // Index to keep track of the current position in morse_code
    int i = 0;

    // While loop to iterate over each character of the Morse code
    while (i < len) {
        // Find the next word in Morse code
        int word_end = i;
        while (morse_code[word_end] != '\0' && !isspace(morse_code[word_end])) {
            ++word_end;
        }

        // Print the word in English
        int j = i;
        while (j < word_end) {
            // Find the next character in Morse code
            int char_end = j;
            while (morse_code[char_end] != '\0' && morse_code[char_end] != ' ') {
                ++char_end;
            }

            // Extract the Morse code for the current character
            char morse_char[6];
            strncpy(morse_char, &morse_code[j], char_end - j);
            morse_char[char_end - j] = '\0';

            // Convert the Morse code to the corresponding English character
            int index = -1;
            for (int k = 0; k < 27; ++k) {
                if (strcmp(morse_char, morse_code_table[k]) == 0) {
                    index = k;
                    break;
                }
            }

            // Print the English character
            if (index != -1) {
                printf("%c", "ABCDEFGHIJKLMNOPQRSTUVWXYZ "[index]);
            }

            // Move to the next character in Morse code
            j = char_end + 1;
        }

        // Move to the next word in Morse code
        i = word_end + 1;
    }
}
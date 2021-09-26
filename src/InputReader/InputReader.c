#include "InputReader.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../SubSetGenerator/SubSetGenerator.h"
#include "../Helpers/ErrorHandler/ErrorHandler.h"

// Dit was ooit een gigantische functie die zelf controleerde op bufferoverflow.
// Blijkt dat er een ingebouwde manier bestaat om dit makkelijk te doen
char* read_input(char* buffer) {
    return fgets(buffer, sizeof(char)*MAX_INPUT_LENGTH, stdin);
}

void split_input(char **destination, char *input_line, int* wordcounter) {
    char *substring = strtok(input_line, " ");
    // Tellen van aantal woorden
    while (substring != NULL) {
        destination[*wordcounter] = malloc((MAX_INPUT_LENGTH + 1) * sizeof(char));
        assert_with_message(destination[*wordcounter] != NULL, SUBSET_MALLOC_FAILED);
        strcpy(destination[*wordcounter], substring);
        substring = strtok(NULL, " ");
        (*wordcounter)++;
    }
}

void remove_trailing_newline(char *input_line) {
    // verwijder newline-char van input
    input_line[strcspn(input_line, "\r\n")] = '\0';
}
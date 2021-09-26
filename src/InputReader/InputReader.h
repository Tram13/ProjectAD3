#ifndef PROJECTAD3_INPUTREADER_H
#define PROJECTAD3_INPUTREADER_H
#define MAX_INPUT_LENGTH 16348 // Vaste limiet op lengte lijn, al denk ik niet dat deze limiet snel zal overschreden worden.
#define TOO_MANY_WORDS_ERROR "Too many words on inputline."

char* read_input(char* buffer);

void split_input(char **destination, char *input_line, int* wordcounter);

void remove_trailing_newline(char *input_line);

#endif //PROJECTAD3_INPUTREADER_H

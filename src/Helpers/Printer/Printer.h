#ifndef PROJECTAD3_PRINTER_H
#define PROJECTAD3_PRINTER_H

#include "../../DatabaseReader/DatabaseReader.h"
#include "../../Searcher/Searcher.h"
#include <stdlib.h>

void print_location(Location *location);

void print_string_array(char **stringarray, int itemcounter);

void print_vectorlist(unsigned long int* vectors, char* alphabet);

void print_bits(unsigned int size, void *ptr, char *text);

void print_best_matches(BestMatches *best_matches);

#endif //PROJECTAD3_PRINTER_H

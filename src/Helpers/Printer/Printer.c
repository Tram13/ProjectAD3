#include "Printer.h"
#include <stdio.h>
#include <string.h>

void print_location(Location *location) {
    printf("--- Printing location ---\n");
    printf("ID: %lu\n", location->id);
    printf("Name: %s\n", location->name);
    printf("Rank: %hhu\n", location->rank);
    printf("Long: %f\n", location->longitude);
    printf("Lat: %f\n", location->latitude);
    printf("-------------------------\n");
}

void print_string_array(char **stringarray, int itemcounter) {
    printf("---- Printing Char** ----\n");
    for (int i = 0; i < itemcounter; i++) {
        printf("%s\n", stringarray[i]);
    }
    printf("-------------------------\n");
}

void print_vectorlist(unsigned long int *vectors, char *alphabet) {
    for (int i = 0; i < strlen(alphabet); i++) {
        printf("Voor letter %c\n", alphabet[i]);
        printf("%lu", vectors[i]);
        printf("\n");
    }
}

void print_bits(unsigned int size, void *ptr, char *text) {
    unsigned char *b = (unsigned char *) ptr;
    printf("%s ", text);
    for (int i = size - 1; i >= 0; i--) {
        printf("%u", (*b >> i) & 1);
    }
    printf("\n");
}

void print_best_matches(BestMatches *best_matches) {
    for (int i = 0; i < AMOUNT_BEST_MATCHES; i++) {
        // Enkel printen indien die bestaat
        if (best_matches->scores[i] != -1) {
            Location *best = get_highest_rank_location(best_matches->all_matches[i]->database->locations,
                                                       best_matches->all_matches[i]->database->size);
            printf("\n");
            printf("%s (%lu)\n", best->name, best->id);
            printf("geo: %f,%f\n", best->latitude, best->longitude);
            printf("~\n");
        }
    }
}
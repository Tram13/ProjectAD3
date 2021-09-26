#ifndef PROJECTAD3_SEARCHER_H
#define PROJECTAD3_SEARCHER_H

#define MAX_ALLOWED_EDIT_DISTANCE 3u
#define EDIT_COST 1
#define MALLOC_SEARCH_RESULTS_FAIL "Searchresults could not be saved in memory."
#define DEFAULT_SEARCH_RESULT_SIZE 100
#define AMOUNT_BEST_MATCHES 5

#include <stdint.h>
#include <stdlib.h>
#include "../SubSetGenerator/SubSetGenerator.h"
#include "../DatabaseReader/DatabaseReader.h"
#include "../ScoreCalculator/ScoreCalculator.h"

typedef struct SearchResults {
    Database *database;
    unsigned int* distance;
    int size;
} SearchResults;

typedef struct BestMatches {
    SearchResults **all_matches;
    double *scores;
    unsigned int size;
} BestMatches;

unsigned int editdistance_substring(char *search_term, char *text);

void copy_list(unsigned int *destination, const unsigned int *source, unsigned int amount);

unsigned int min3(unsigned int a, unsigned int b, unsigned int c);

SearchResults *do_search(OrderedDatabase *database, char *search_term, SearchResults **searchresults, int current_substring_count);

void
get_best_matches(SearchResults **searchresults, StringList *stringlist, BestMatches *best_matches, Arguments *args);

BestMatches *init_best_matches();

unsigned int i_min(unsigned int x, unsigned int y);

void save_searchresult(Location *location, int *current_searchresults_size, SearchResults *sr, unsigned int distance);

SearchResults *init_current_searchresults(SearchResults **searchresults, int current_substring_count);

void free_searchresults(SearchResults **searchresults, int size);

int index_new_best_match(BestMatches *best_matches, double score);

void save_score(Location ** total_match, int size, BestMatches *best_matches, int index, double score, const unsigned int* distance);

void free_best_matches(BestMatches *best_matches);

Location *get_highest_rank_location(Location **locations, unsigned int size);

uint32_t *clean_string(char * string);

void sort_best_matches(BestMatches *best_matches);

#endif //PROJECTAD3_SEARCHER_H
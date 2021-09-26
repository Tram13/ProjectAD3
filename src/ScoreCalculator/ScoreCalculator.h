#ifndef PROJECTAD3_SCORECALCULATOR_H
#define PROJECTAD3_SCORECALCULATOR_H
#include "../DatabaseReader/DatabaseReader.h"
#include "../SubSetGenerator/SubSetGenerator.h"
#include "../ArgumentParser/ArgumentParser.h"

#define R 6371
#define TO_RAD (3.1415926536 / 180)

double get_synergy(Location ** total_match, int size);

double calculate_score_of_two(Location *match1, Location *match2);

double get_extend(Location *match);

double d_geo(Location *match1, Location *match2);

double get_correctness(const unsigned int* distance, StringList *stringlist);

double get_max_rank(Location ** total_match, int size);

double get_coords_score(Location** current_match, Arguments *args, int size);

double get_score(Location ** current_match, const unsigned int* distance, StringList *stringlist, Arguments *args);

double max(double x, double y);

double min(double x, double y);

#endif //PROJECTAD3_SCORECALCULATOR_H

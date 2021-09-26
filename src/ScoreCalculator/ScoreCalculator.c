#include <math.h>
#include <string.h>
#include "ScoreCalculator.h"
#include "../Searcher/Searcher.h"

const double RANK_TO_EXTEND[] = {100000, 50000, 10000, 1000, 750, 600, 500, 350, 200, 100, 75, 50, 10, 9, 7.5, 6, 4.5, 4, 2, 1, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1, 0.05, 0.01};

double get_synergy(Location ** total_match, int size) {
    // Score berekenen
    if (size == 1) {
        return 0;
    }
    double total_score = 0;
    for (int i = 0; i < size; i++) {
        for (int j = i + 1; j < size; j++) {
            total_score += calculate_score_of_two(total_match[i], total_match[j]);
        }
    }
    total_score /= size * (size - 1);
    return total_score;
}

double calculate_score_of_two(Location *match1, Location *match2) {
    double score = max(get_extend(match1), get_extend(match2)) / d_geo(match1, match2);
    return min(2, pow(score, 2));
}

double get_extend(Location *match) {
    return RANK_TO_EXTEND[match->rank - 1];
}

double d_geo(Location *match1, Location *match2) {
    double lat1 = match1->latitude;
    double lon1 = match1->longitude;
    double lat2 = match2->latitude;
    double lon2 = match2->longitude;
    // Code van Ufora
    double dx, dy, dz;
    lon1 = lon1 - lon2;
    lon1 *= TO_RAD, lat1 *= TO_RAD, lat2 *= TO_RAD;

    dz = sin(lat1) - sin(lat2);
    dx = cos(lon1) * cos(lat1) - cos(lat2);
    dy = sin(lon1) * cos(lat1);
    double distance = asin(sqrt(dx * dx + dy * dy + dz * dz) / 2) * 2 * R; // km
    return distance;
}

double get_correctness(const unsigned int* distance, StringList *stringlist) {
    double dividend = 0;
    double divisor = 0;
    for (int i = 0; i < stringlist->size; i++) {
        dividend += (double) strlen(stringlist->list[i]) - (double) distance[i];
        divisor += (int) strlen(stringlist->list[i]);
    }
    return dividend / divisor;
}

double get_max_rank(Location ** total_match, int size) {
    double max_rank = 0;
    for (int i = 0; i < size; i++) {
        max_rank = max(total_match[i]->rank, max_rank);
    }
    return max_rank / 30;
}

double get_coords_score(Location** current_match, Arguments *args, int size) {
    double score;
    Location *most_specific = get_highest_rank_location(current_match, size);
    Location arg_location = {0, "", 0, args->lat, args->lon}; // Dummy-struct met enkel relevante waarden ingevuld
    double geo_distance = d_geo(most_specific, &arg_location);
    if (geo_distance == 0) {
        return 1;
    }
    double extend = get_extend(most_specific);
    score = geo_distance / extend;
    score = 1 - (log(score) / log(1000));
    score = max(score, 0);
    score = min(1, score);
    return score;
}

double get_score(Location ** current_match, const unsigned int* distance, StringList *stringlist, Arguments *args) {
    int size = stringlist->size;
    double synergy = get_synergy(current_match, size);
    double correctness = get_correctness(distance, stringlist);
    double max_rank = get_max_rank(current_match, size);
    if (args->lat == LAT_LON_NOT_SET) {
        return 0.2 * correctness + 0.7 * synergy + 0.1 * max_rank;
    } else {
        double coords_score = get_coords_score(current_match, args, size);
        return 0.15 * correctness + 0.55 * synergy + 0.15 * max_rank + 0.15 * coords_score;
    }
}

double max(double x, double y) {
    return x * (x >= y) + y * (y > x);
}

double min(double x, double y) {
    return x * (x <= y) + y * (y < x);
}
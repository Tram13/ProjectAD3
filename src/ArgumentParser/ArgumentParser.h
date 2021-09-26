#ifndef PROJECTAD3_ARGUMENTPARSER_H
#define PROJECTAD3_ARGUMENTPARSER_H

#define LAT_LON_NOT_SET 500 // Een geldige waarde ligt altijd tussen -90->90 en -180->180.
                            // 500 is dus een random ongeldige waarde.
#define ARGS_PARSE_FAIL_ERROR "Usage: openseekmap database [lat lon]"
#define ARGS_MALLOC_FAIL_ERROR "Not enough memory to parse arguments"

typedef struct Arguments {
    char *database;
    double lat;
    double lon;
} Arguments;

Arguments *parse_arguments(int argc, char **argv);

void free_arguments(Arguments *args);

#endif //PROJECTAD3_ARGUMENTPARSER_H

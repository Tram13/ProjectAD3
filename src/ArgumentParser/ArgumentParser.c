#include <stdlib.h>
#include "ArgumentParser.h"
#include "../Helpers/ErrorHandler/ErrorHandler.h"

Arguments *parse_arguments(int argc, char **argv) {
    Arguments *arguments = malloc(sizeof(Arguments));
    assert_with_message(arguments != NULL, ARGS_MALLOC_FAIL_ERROR);

    // Verwerk programma-argumenten
    // We vereisen 1 argument (databank) of 3 argumenten
    assert_with_message((argc == 2 || argc == 4), ARGS_PARSE_FAIL_ERROR);
    arguments->database = argv[1];
    if (argc == 4) { // Er worden 3 argumenten meegegeven (databank, lat, lon)
        arguments->lat = strtod(argv[2], NULL);
        arguments->lon = strtod(argv[3], NULL);
    } else {
        arguments->lat = LAT_LON_NOT_SET;
        arguments->lon = LAT_LON_NOT_SET;
    }
    return arguments;
}

void free_arguments(Arguments *args) {
    free(args);
}

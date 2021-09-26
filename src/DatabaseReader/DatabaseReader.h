#ifndef PROJECTAD3_DATABASEREADER_H
#define PROJECTAD3_DATABASEREADER_H
#define MALLOC_FAIL "Could not allocate memory for all database entries."
#define MAX_LINE_LENGTH 2048
#define FILE_NOT_FOUND_ERROR "Could not open file."
#define INVALID_FILE_CONTENT_ERROR "The database content does not follow the required template."
#define DEFAULT_DATABASE_SIZE 100u

#include <stdint.h>

typedef struct Location {
    uint64_t id;
    char *name;
    uint8_t rank;
    double latitude;
    double longitude;
} Location;

typedef struct Database {
    Location **locations;
    unsigned int size;
} Database;

typedef struct OrderedDatabase {
    Database **databases;
    unsigned int size;
} OrderedDatabase;

OrderedDatabase *read_database(char *filename);

Location *create_location(char *buffer);

void free_ordered_database(OrderedDatabase *ordered_database);

#endif //PROJECTAD3_DATABASEREADER_H


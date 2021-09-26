#include "DatabaseReader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Helpers/ErrorHandler/ErrorHandler.h"
#include "../Helpers/UTF8/utf8.h"

OrderedDatabase *read_database(char *filename) {
    FILE *fptr = fopen(filename, "r");
    assert_with_message(fptr != NULL, FILE_NOT_FOUND_ERROR);

    int buffer_length = MAX_LINE_LENGTH; // Willekeurig gekozen groot getal, ik verwacht zeker niet dat er lijnen zullen zijn met meer dan MAX_LINE_LENGTH tekens.
    char buffer[buffer_length];

    // Als we zouden meten hoelang de grootste lijn uit de databank is, moeten we minder mallocen -> dus geheugengebruik
    // zou dan ook efficienter zijn. Echter kunnen we niet op voorhand kijken hoe groot de langste lijn is, want bij
    // grote databanken (zoals planet.data) zou de preprocessing veel te lang duren.
    unsigned int ordered_sizes[MAX_LINE_LENGTH + 1]; // +1 omdat we 0 ook meerekenen
    OrderedDatabase *ordered_database = malloc(sizeof(OrderedDatabase));
    assert_with_message(ordered_database != NULL, MALLOC_FAIL);
    ordered_database->databases = malloc((MAX_LINE_LENGTH + 1) * sizeof(Database *));
    assert_with_message(ordered_database->databases != NULL, MALLOC_FAIL);
    ordered_database->size = MAX_LINE_LENGTH + 1;

    // Initialiseren van OrderedDatabase
    for (int i = 0; i <= MAX_LINE_LENGTH; i++) {
        ordered_database->databases[i] = malloc(sizeof(Database));
        assert_with_message(ordered_database->databases[i] != NULL, MALLOC_FAIL);
        ordered_sizes[i] = DEFAULT_DATABASE_SIZE;
        ordered_database->databases[i]->locations = malloc(DEFAULT_DATABASE_SIZE * sizeof(Location *));
        assert_with_message(ordered_database->databases[i]->locations != NULL, MALLOC_FAIL);
        ordered_database->databases[i]->size = 0;
    }

    while (fgets(buffer, buffer_length, fptr)) { // Inlezen, lijn per lijn, max MAX_LINE_LENGTH chars per lijn
        // Creatie van Location-struct, dit stelt een lijntje uit de databank voor
        Location *new_location = create_location(buffer);
        // Juiste plaats in databank zoeken
        unsigned int name_length = u8_strlen(new_location->name);
        Database *current_database = ordered_database->databases[name_length];
        // Locatie opslaan in juiste databank
        current_database->locations[current_database->size] = new_location;
        current_database->size++;
        // Indien nodig, plaats in die databank vergroten
        if (ordered_sizes[name_length] <= current_database->size) {
            ordered_sizes[name_length] *= 2; // Verdubbel de grootte van de databank
            current_database->locations = realloc(current_database->locations, ordered_sizes[name_length] * sizeof(Location *));
            assert_with_message(current_database->locations != NULL, MALLOC_FAIL);
        }
    }
    fclose(fptr);
    return ordered_database;
}

Location *create_location(char *buffer) {
    const char delimiter[sizeof("\t")] = "\t";
    // Inlezen ID
    uint64_t osm_id = strtoul(strtok(buffer, delimiter), NULL, 0);
    // (Geen controle op ID, want die kan technisch gezien 0 zijn)
    // Inlezen Name
    char *name_temp = strtok(NULL, delimiter);
    assert_with_message(name_temp != NULL, INVALID_FILE_CONTENT_ERROR);
    unsigned int name_len = strlen(name_temp);
    char *name = (char *) malloc((name_len + 4) * sizeof(char)); // +4 voor "\0"-en
    assert_with_message(name != NULL, MALLOC_FAIL);
    strcpy(name, name_temp);
    for (int i = 0; i < 4; i++) {
        name[name_len + i] = '\0';
    }
    // Inlezen Rank
    uint8_t rank = (uint8_t) strtol(strtok(NULL, delimiter), NULL, 0);
    // Geen controle op rang, want die kan 0 zijn (zie lijn 108 van planet.data)
    // Opgelet: de databank gaat in tegen de standaard EPSG:4326: latitude, longitude
    // Inlezen Longitude
    double longitude = strtod(strtok(NULL, delimiter), NULL);
    // Inlezen latitude
    double latitude = strtod(strtok(NULL, delimiter), NULL);
    // (Geen controle op latitude en longitude, want die kunnen technisch gezien 0 zijn

    // Creatie van Location-struct
    Location *new_location = malloc(sizeof(Location));
    assert_with_message(new_location != NULL, MALLOC_FAIL);

    // Location-struct invullen met ingelezen data
    new_location->id = osm_id;
    new_location->name = name;
    new_location->rank = rank;
    new_location->longitude = longitude;
    new_location->latitude = latitude;

    return new_location;
}

void free_ordered_database(OrderedDatabase *ordered_database) {
    for (int i = 0; i < ordered_database->size; i++) {
        Database *database = ordered_database->databases[i];
        for (int j = 0; j < database->size; j++) {
            // Free alle namen
            free(database->locations[j]->name);
            // Free location
            free(database->locations[j]);
        }
        // Free locations lijst
        free(database->locations);
        // Free de databank struct
        free(database);
    }
    free(ordered_database->databases);
    free(ordered_database);
}
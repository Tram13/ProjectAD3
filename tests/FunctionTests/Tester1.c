#include <string.h>
#include <stdlib.h>
#include "Tester1.h"
#include "../../src/ArgumentParser/ArgumentParser.h"
#include "../../src/Helpers/ErrorHandler/ErrorHandler.h"
#include "../../src/SubSetGenerator/SubSetGenerator.h"
#include "../../src/InputReader/InputReader.h"
#include "../../src/Searcher/Searcher.h"
#include <stdio.h>


int test_parse_arguments(int argc, char **argv) {
    int is_correct;

    // Met volledig ingevulde parameters
    Arguments *args_should_be_with_coords = parse_arguments(argc, argv);

    is_correct = strcmp(args_should_be_with_coords->database, "database.data") == 0;
    assert_with_message(is_correct, ARGS_TEST_FAILED);

    is_correct = args_should_be_with_coords->lon == 3.7107158;
    assert_with_message(is_correct, ARGS_TEST_FAILED);

    is_correct = args_should_be_with_coords->lat == 51.0355237;
    assert_with_message(is_correct, ARGS_TEST_FAILED);

    // Met enkel databank als parameter
    argc = 2;
    argv[2] = "";
    argv[3] = "";
    Arguments *args_no_coords = parse_arguments(argc, argv);

    is_correct = strcmp(args_no_coords->database, "database.data") == 0;
    assert_with_message(is_correct, ARGS_TEST_FAILED);

    is_correct = args_no_coords->lat == LAT_LON_NOT_SET;
    assert_with_message(is_correct, ARGS_TEST_FAILED);

    is_correct = args_no_coords->lon == LAT_LON_NOT_SET;
    assert_with_message(is_correct, ARGS_TEST_FAILED);

    return 0;
}

int test_substring_generator() {
    char input_line[] = "De Sterre Gent";
    char oplossingen[8][15] = {"De", "Sterre", "Gent", "De Sterre", "Gent", "De", "Sterre Gent", "De Sterre Gent"};
    int is_correct;

    // Vars initialiseren
    int wordcounter = 0;
    char *splitted[MAX_INPUT_LENGTH];

    // Code die substrings-boom maakt
    split_input(splitted, input_line, &wordcounter);
    SubSets subsets = {NULL, 0};
    Node root = {0, NULL, malloc(sizeof(Node *) * wordcounter), 0};
    generate_subsets(&subsets, wordcounter, &root);


    int counter = 0;
    // Omzetten naar strings
    for (int i = 0; i < subsets.possibilities; i++) {
        NodeList *path = subsets.paths[i];
        // Genereer de effectieve string
        char *list[path->size];
        StringList stringlist = {list, 0};
        get_stringlist_from_path(path, &stringlist, splitted);
        for (int j = 0; j < stringlist.size; j++) {
            is_correct = strcmp(stringlist.list[j], oplossingen[counter]) == 0;
            assert_with_message(is_correct, SUBSTRING_GENERATOR_TEST_FAILED);
            counter++;
        }
    }
    return 0;
}

int test_i_min() {
    int is_correct;

    is_correct = i_min(1, 1) == 1;
    assert_with_message(is_correct, I_MIN_TEST_FAILED);

    is_correct = i_min(2, 3) == 2;
    assert_with_message(is_correct, I_MIN_TEST_FAILED);

    is_correct = i_min(5, 4) == 4;
    assert_with_message(is_correct, I_MIN_TEST_FAILED);

    is_correct = i_min(0, 0) == 0;
    assert_with_message(is_correct, I_MIN_TEST_FAILED);

    return 0;
}

int test_clean_string() {
    int is_correct;
    uint32_t *cleaned;

    char hello_world[] = "Hello World";
    char hello_world_oplossing[] = "hello world";
    cleaned = clean_string(hello_world);
    for (int i = 0; i < strlen(hello_world_oplossing); i++) {
        is_correct = hello_world_oplossing[i] == (char) cleaned[i];
        assert_with_message(is_correct, CLEAN_STRING_TEST_FAILED);
    }
    free(cleaned);

    char belgie[] = "België ïś éèñ lánd";
    char belgie_oplossing[] = "belgie is een land";
    cleaned = clean_string(belgie);
    for (int i = 0; i < strlen(belgie_oplossing); i++) {
        is_correct = belgie_oplossing[i] == (char) cleaned[i];
        assert_with_message(is_correct, CLEAN_STRING_TEST_FAILED);
    }
    free(cleaned);

    char random_letters[] = "ëEËfahgtyàçéhàé";
    char random_letters_oplossing[] = "eeefahgtyacehae";
    cleaned = clean_string(random_letters);
    for (int i = 0; i < strlen(random_letters_oplossing); i++) {
        is_correct = random_letters_oplossing[i] == (char) cleaned[i];
        assert_with_message(is_correct, CLEAN_STRING_TEST_FAILED);
    }
    free(cleaned);

    return 0;
}

int test_editdistance_substring() {
    int is_correct;
    unsigned int distance;
    // Max toegestane editeerafstand is 3, deze functie geeft geen waarde hoger dan MAX_ALLOWED_EDIT_DISTANCE + 1 terug
    distance = editdistance_substring("vrijheidstraat brugge", "vrijheidsstraat brugge"); // Toevoeging
    is_correct = distance == 1;
    assert_with_message(is_correct, EDITDISTANCE_SUBSTRING_TEST_FAILED);

    distance = editdistance_substring("vrijheidsstraat brugge", "vrijheidstraat brugge"); // Verwijdering
    is_correct = distance == 1;
    assert_with_message(is_correct, EDITDISTANCE_SUBSTRING_TEST_FAILED);

    distance = editdistance_substring("vrijheidsstraat brugge", "vrijheidsstriat brugge"); // Substitutie
    is_correct = distance == 1;
    assert_with_message(is_correct, EDITDISTANCE_SUBSTRING_TEST_FAILED);

    distance = editdistance_substring("vrijheidstsraat brugge", "vrijheidsstraat brugge"); // Verwisseling
    is_correct = distance == 1;
    assert_with_message(is_correct, EDITDISTANCE_SUBSTRING_TEST_FAILED);

    distance = editdistance_substring("itit", "titi"); // Meerdere oplossingen, 2 aanpassingen, bvb 2x verwisseling
    is_correct = distance == 2;
    assert_with_message(is_correct, EDITDISTANCE_SUBSTRING_TEST_FAILED);

    distance = editdistance_substring("itititit",
                                      "titititi"); // Voeg toe en verwijder letter in begin en eind respectivelijk, of omgekeerd
    is_correct = distance == 2;
    assert_with_message(is_correct, EDITDISTANCE_SUBSTRING_TEST_FAILED);

    distance = editdistance_substring("azerty", "qwerty");
    is_correct = distance == 2;
    assert_with_message(is_correct, EDITDISTANCE_SUBSTRING_TEST_FAILED);

    distance = editdistance_substring("abcdefg", "hijklm");
    is_correct = distance == MAX_ALLOWED_EDIT_DISTANCE + 1;
    assert_with_message(is_correct, EDITDISTANCE_SUBSTRING_TEST_FAILED);

    distance = editdistance_substring("sterre", "siberie");
    is_correct = distance == 3;
    assert_with_message(is_correct, EDITDISTANCE_SUBSTRING_TEST_FAILED);

    return 0;
}

int test_min3() {
    int is_correct;

    is_correct = min3(1, 1, 1) == 1;
    assert_with_message(is_correct, MIN3_TEST_FAILED);

    is_correct = min3(2, 3, 2) == 2;
    assert_with_message(is_correct, MIN3_TEST_FAILED);

    is_correct = min3(5, 4, 3) == 3;
    assert_with_message(is_correct, MIN3_TEST_FAILED);

    is_correct = min3(0, 0, 0) == 0;
    assert_with_message(is_correct, MIN3_TEST_FAILED);

    is_correct = min3(8, 7, 9) == 7;
    assert_with_message(is_correct, MIN3_TEST_FAILED);

    is_correct = min3(3000, 2000, 3000) == 2000;
    assert_with_message(is_correct, MIN3_TEST_FAILED);

    return 0;
}
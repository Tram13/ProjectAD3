#ifndef OPENSEEKMAP_TESTER_H
#define OPENSEEKMAP_TESTER_H

#define ARGS_TEST_FAILED "Failed to parse arguments"
#define SUBSTRING_GENERATOR_TEST_FAILED "Failed to generate substrings"
#define I_MIN_TEST_FAILED "Failed to get mininum of 2 unsigned ints"
#define CLEAN_STRING_TEST_FAILED "Failed to clean string"
#define EDITDISTANCE_SUBSTRING_TEST_FAILED "Failed to get editdistance substring"
#define MIN3_TEST_FAILED "Failed to get minimum of 3 unsigned ints"

int test_parse_arguments(int argc, char **argv);

int test_substring_generator();

int test_i_min();

int test_clean_string();

int test_editdistance_substring();

int test_min3();

#endif //OPENSEEKMAP_TESTER_H

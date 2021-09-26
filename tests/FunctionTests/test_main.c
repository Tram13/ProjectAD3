#include "Tester1.h"
#include "Tester2.h"


int main(int argc, char **argv) {
    int success = 0;
    success += test_parse_arguments(argc, argv);
    success += test_substring_generator();
    success += test_i_min();
    success += test_clean_string();
    success += test_editdistance_substring();
    success += test_min3();
    success += test_max();
    success += test_min();

    return success;
}

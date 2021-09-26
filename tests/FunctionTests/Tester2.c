#include "Tester2.h"
#include "../../src/Helpers/ErrorHandler/ErrorHandler.h"
#include "../../src/ScoreCalculator/ScoreCalculator.h"

int test_max() {
    int is_correct;

    is_correct = max(1, 1) == 1;
    assert_with_message(is_correct, MAX_TEST_FAILED);

    is_correct = max(2, 3) == 3;
    assert_with_message(is_correct, MAX_TEST_FAILED);

    is_correct = max(5, 4) == 5;
    assert_with_message(is_correct, MAX_TEST_FAILED);

    is_correct = max(0, 0) == 0;
    assert_with_message(is_correct, MAX_TEST_FAILED);

    is_correct = max(1.0, 1) == 1;
    assert_with_message(is_correct, MAX_TEST_FAILED);

    is_correct = max(1.5, 2.5) == 2.5;
    assert_with_message(is_correct, MAX_TEST_FAILED);

    is_correct = max(3.141592, 3.141593) == 3.141593;
    assert_with_message(is_correct, MAX_TEST_FAILED);

    is_correct = max(1.5, -2.5) == 1.5;
    assert_with_message(is_correct, MAX_TEST_FAILED);

    is_correct = max(0.99999999, 1) == 1;
    assert_with_message(is_correct, MAX_TEST_FAILED);

    is_correct = max(1.00000001, 1) == 1.00000001;
    assert_with_message(is_correct, MAX_TEST_FAILED);

    return 0;
}

int test_min() {
    int is_correct;

    is_correct = min(1, 1) == 1;
    assert_with_message(is_correct, MIN_TEST_FAILED);

    is_correct = min(2, 3) == 2;
    assert_with_message(is_correct, MIN_TEST_FAILED);

    is_correct = min(5, 4) == 4;
    assert_with_message(is_correct, MIN_TEST_FAILED);

    is_correct = min(0, 0) == 0;
    assert_with_message(is_correct, MIN_TEST_FAILED);

    is_correct = min(1.0, 1) == 1;
    assert_with_message(is_correct, MIN_TEST_FAILED);

    is_correct = min(1.5, 2.5) == 1.5;
    assert_with_message(is_correct, MIN_TEST_FAILED);

    is_correct = min(3.141592, 3.141593) == 3.141592;
    assert_with_message(is_correct, MIN_TEST_FAILED);

    is_correct = min(1.5, -2.5) == -2.5;
    assert_with_message(is_correct, MIN_TEST_FAILED);

    is_correct = min(0.99999999, 1) == 0.99999999;
    assert_with_message(is_correct, MIN_TEST_FAILED);

    is_correct = min(1.00000001, 1) == 1;
    assert_with_message(is_correct, MIN_TEST_FAILED);

    return 0;
}

#include "ErrorHandler.h"
#include <stdio.h>
#include <assert.h>

void assert_with_message(int is_valid, char* message) { // Boolean en bericht wanneer boolean false is
    if (!is_valid) {
        printf("%s\n", message);
        assert(0); // Geef error als assert faalt
    }
}
